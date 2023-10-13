#version 450 core

in vec4 clipPos;
in vec4 ndcPos;
in vec3 tang;
in vec3 worldNorm;
in vec4 col;

in vec2 uv0;

in vec3 worldNormal;

out vec4 FragColor;

// mainLight
struct mainParallelLight{
    vec4 ColorAndStrength;
    vec3 Direction;
};
uniform mainParallelLight mLight;
uniform bool useMainLight;

// pointLights
#define POINT_LIGHT_MAX_NUMBER 16

struct pointLight{
    vec4 ColorAndStrength;
    vec3 Position;
};
uniform pointLight pLight[POINT_LIGHT_MAX_NUMBER];
uniform int usedPointLightNumber;

// duffuse/albedo
uniform sampler2D diffuseOrAlbedo;
uniform vec4 diffuseOrAlbedoVec;
uniform bool diffuseOrAlbedoUseTex;

// metallic-roughness-alpha-subsurface
uniform sampler2D mras;
uniform vec4 mrasVec;
uniform bool mrasUseTex;
// specular-specularTint
uniform sampler2D specularAndTint;
uniform vec4 specularAndTintVec;
uniform bool specularAndTintUseTex;
// sheen-sheenTint-clearCoat-clearCoatTint
uniform sampler2D sheenAndTintWithclearCoatAndGloss;
uniform vec4 sheenAndTintWithclearCoatAndGlossVec;
uniform bool sheenAndTintWithclearCoatAndGlossUseTex;


// normal-ao
uniform sampler2D normalAO;
uniform vec4 normalAOVec;
uniform bool normalAOUseTex;


// Luminance
uniform sampler2D Luminance;
uniform vec4 LuminanceVec;
uniform bool LuminanceUseTex;
// Transparency
uniform sampler2D Transparency;
uniform vec4 TransparencyVec;
uniform bool TransparencyUseTex;



uniform mat4 model_inverse;
// 摄像机参数
uniform mat4 view_inverse;
uniform mat4 proj_inverse;

uniform vec3 cameraPos;

uniform float near;
uniform float far;

#define PI 3.14159265358979323846

// 重建世界坐标
vec3 rebuildWorldPos(){
    // // 重建远平面点在世界空间的位置
    vec4 farPlaneClipPos = vec4(ndcPos.xy, 1, 1) * far;
    vec3 farPlaneWorldPos = (view_inverse * proj_inverse * farPlaneClipPos).xyz;
    // 根据重建的位置计算光线方向
    vec3 rayDir = normalize(farPlaneWorldPos - cameraPos);
    return cameraPos + rayDir * clipPos.z;
}
// 读取法线纹理
vec3 unpackNormal(vec3 rawNormal){
    vec3 ansNormal;
    ansNormal.xy = rawNormal.xy * 2.0 - 1.0;
    ansNormal.z = sqrt(1 - min(1, dot(ansNormal.xy, ansNormal.xy)));
    return normalize(ansNormal);
}

float Pow2(float x);
float Pow5(float x);

// // Diffuse项
// vec3 Diffuse(vec3 baseColor, float Difference, float NormalDotLight, float NormalDotView, float roughness);

// // specular-D项
// float D_GGX(float NormalDotHalf, float roughness);

// // specular-Fresnel项
// vec3 F_Schlick(vec3 F0, float ViewDotHalf);

// // schlickGGX
// float G_SchlickGGX(float NormalDotView, float roughness, bool isIBL);
// // specular-G项 
// float G_Smith(float NormalDotView, float NormalDotLight, float roughness, bool isIBL);

// Disney_BRDF
vec3 Disney_BRDF    (
    vec3 lightDir, 
    vec3 viewDir, 
    vec3 normalDir, 
    vec3 baseColor, 
    float metalness, 
    float roughness,
    bool isIBL,
    float anisotropic,
    float subsurface,
    float specular,
    float specularTint,
    float sheen,
    float sheenTint,
    float clearCoat,
    float clearCoatGloss
    );


void main(){
    vec3 worldPos = rebuildWorldPos();

    vec3 albedo;

    vec4 detailNormalAO;
    float ao;

    float metallic;
    float roughness;
    float anisotropic;
    float subsurface;

    float specular;
    float specularTint;

    float sheen;
    float sheenTint;

    float clearCoat;
    float clearCoatGloss;

    //首先读取系列纹理
    if (diffuseOrAlbedoUseTex)
        albedo = texture(diffuseOrAlbedo, uv0).xyz;
    else
        albedo = diffuseOrAlbedoVec.xyz;
    
    if (normalAOUseTex)
        detailNormalAO = texture(normalAO, uv0);
    else
        detailNormalAO = normalAOVec;

    if (mrasUseTex){
        vec4 mrasV = texture(mras, uv0);
        metallic = mrasV.x;
        roughness = mrasV.y;
        anisotropic = mrasV.z;
        subsurface = mrasV.w;
    }else{
        metallic = mrasVec.x;
        roughness = mrasVec.y;
        anisotropic = mrasVec.z;
        subsurface = mrasVec.w;
    }

    if (specularAndTintUseTex){
        vec2 satV = texture(specularAndTint, uv0).xy;
        specular = satV.x;
        specularTint = satV.y;
    }else {
        specular = specularAndTintVec.x;
        specularTint = specularAndTintVec.y;
    }

    if (sheenAndTintWithclearCoatAndGlossUseTex){
        vec4 stcg = texture(sheenAndTintWithclearCoatAndGloss, uv0);
        sheen = stcg.x;
        sheenTint = stcg.y;
        clearCoat = stcg.z;
        clearCoatGloss = stcg.w;
    }else{
        sheen = sheenAndTintWithclearCoatAndGlossVec.x;
        sheenTint = sheenAndTintWithclearCoatAndGlossVec.y;
        clearCoat = sheenAndTintWithclearCoatAndGlossVec.z;
        clearCoatGloss = sheenAndTintWithclearCoatAndGlossVec.w;
    }

    // 可能的gamma反矫正
    // albedo = pow(albedo, vec3(1.0 / 2.2));

    // 计算重要的变量

    // 副切线
    vec3 biTang = normalize(cross(tang, worldNorm));
    // 切线变换矩阵
    mat3 tang2World;
    tang2World[0] = vec3(tang.x, biTang.x, worldNorm.x);
    tang2World[1] = vec3(tang.y, biTang.y, worldNorm.y);
    tang2World[2] = vec3(tang.z, biTang.z, worldNorm.z);

    tang2World = mat3(tang, biTang, worldNorm);
    // 变换后的法线
    vec3 unpackedNormal = normalize(unpackNormal(detailNormalAO.xyz));
    vec3 worldDetailedNorm = normalize(tang2World * unpackedNormal.xyz);

    // ao叠加
    albedo *= detailNormalAO.w;

    // 观察方向
    vec3 viewDir = normalize(cameraPos - worldPos.xyz);

    vec3 ans = vec3(0);
    // 首先计算主光源平行光
    if (useMainLight) {
        ans += Disney_BRDF(mLight.Direction, viewDir, worldDetailedNorm, albedo, metallic, roughness, false, anisotropic, subsurface, specular, specularTint, sheen, sheenTint, clearCoat, clearCoatGloss) * max(0, dot(mLight.Direction, worldDetailedNorm)) * mLight.ColorAndStrength.xyz * mLight.ColorAndStrength.w;
    }
    // 点光源
    for(int i = 0; i < min(usedPointLightNumber, POINT_LIGHT_MAX_NUMBER); ++i){
        float dist = length(pLight[i].Position - worldPos.xyz);
        vec3 lightDir = normalize(pLight[i].Position - worldPos.xyz);

        ans += Disney_BRDF(lightDir, viewDir, worldDetailedNorm, albedo, metallic, roughness, false, anisotropic, subsurface, specular, specularTint, sheen, sheenTint, clearCoat, clearCoatGloss) * max(0, dot(lightDir, worldDetailedNorm)) * pLight[i].ColorAndStrength.xyz * pLight[i].ColorAndStrength.w * exp(-dist);
    }

    FragColor = vec4(ans.xyz, 1.0);

    // testAndDebug
    // FragColor = vec4(worldDetailedNorm.xyz, 1.0);

    // float dist = length(pLight[0].Position - worldPos.xyz);
    // FragColor = vec4(dist.xxx, 1.0);

    // FragColor = vec4(sheenTint.xxx, 1.0);
}


float Pow2(float x){
    return x * x;
}

float Pow3(float x){
    return x * x * x;
}

float Pow5(float x){
    float x2 = x * x;
    return x2 * x2 * x;
}

// 漫反射
vec3 Diffuse(vec3 baseColor, float Difference, float NormalDotLight, float NormalDotView, float roughness){
    float FD90 = 0.5 + 2.0 * roughness * Pow2(Difference);
    return (baseColor / PI) * (1.0 + (FD90 - 1.0) * Pow5(1 - NormalDotLight)) * (1.0 + (FD90 - 1.0) * Pow5(1 - NormalDotView));
}

// 次表面散射
vec3 Subsurface(vec3 baseColor, float Difference, float NormalDotLight, float NormalDotView, float roughness){
    float FSS90 = roughness * Pow2(Difference);
    float FSS = (1.0 + (FSS90 - 1.0) * Pow5(1 - NormalDotLight)) * (1.0 + (FSS90 - 1.0) * Pow5(1 - NormalDotView));
    return (baseColor / PI) * 1.25 * (FSS * (1.0 / (NormalDotLight + NormalDotView) - 0.5) + 0.5);
}

// 计算颜色明度
float CalColorLuminance(vec3 color){
    return color.r * 0.2126 + color.g * 0.7152 + color.b * 0.0072;
}

// 光泽度项
vec3 Sheen(vec3 baseColor, float Difference, float sheen, float sheenTint){
    return (vec3(1.0) * (1.0 - sheenTint) + baseColor * sheenTint) * sheen * Pow5(1.0 - Difference);
}


float D_GGX(float NormalDotHalf, float roughness){
    float a2 = Pow2(roughness);
    float denom = PI * Pow2(Pow2(NormalDotHalf) * (a2 - 1) + 1);
    return a2 / denom;
}

vec3 F_Schlick(vec3 F0, float ViewDotHalf){
    float Fc = Pow5(1.0 - ViewDotHalf);
    vec3 F90 = vec3(clamp(50.0 * F0.g, 0.0, 1.0));
    return F90 * Fc + F0 * Fc;//(1 - Fc);    // 究竟是乘1-Fc还是Fc
}

float G_SchlickGGX(float NormalDotView, float roughness, bool isIBL){
    float k;
    if (isIBL){
        k = Pow2(roughness) / 2.0;
    }
    else {
        k = Pow2(roughness + 1.0) / 8.0;
    }
    // float NdotV = max(dot(n, v), 0) + 0.001;
    return NormalDotView / (NormalDotView * (1 - k) + k);
}

float G_Smith(float NormalDotView, float NormalDotLight, float roughness, bool isIBL){
    return G_SchlickGGX(NormalDotView, roughness, isIBL) * G_SchlickGGX(NormalDotLight, roughness, isIBL);
}

vec3 Disney_BRDF
    (
    vec3 lightDir, 
    vec3 viewDir, 
    vec3 normalDir, 
    vec3 baseColor, 
    float metalness, 
    float roughness,
    bool isIBL,
    float anisotropic,
    float subsurface,
    float specular,
    float specularTint,
    float sheen,
    float sheenTint,
    float clearCoat,
    float clearCoatGloss
    ) {

    vec3 L = normalize(lightDir);
    vec3 V = normalize(viewDir);
    vec3 N = normalize(normalDir);
    vec3 H = normalize(L + V);

    vec3 Fdiffuse;
    vec3 Fsubsurface;
    vec3 Fsheen;
    vec3 FcookTorranceSpecular;
    vec3 FclearCoat;
    
    float NoH = dot(N, H);
    float NoV = dot(N, V);
    float NoL = dot(N, L);
    float Diff = dot(L, H);

    // 漫反射项
    Fdiffuse = Diffuse(baseColor, Diff, NoL, NoV, roughness);
    // 次表面项
    Fsubsurface = Subsurface(baseColor, Diff, NoL, NoV, roughness);

    // 光泽度项
    Fsheen = Sheen(baseColor, Diff, sheen, sheenTint);

    // cook Torrance高光项
    float D = D_GGX(NoH, roughness);
    vec3 F0 = vec3(0.02);
    F0 = metalness * baseColor + (1.0 - metalness) * F0;
    vec3 F = F_Schlick(F0, Diff);
    float G = G_Smith(NoV, NoL, roughness, isIBL);

    FcookTorranceSpecular = D * F * G / (4 * NoL * NoV);

    // 清漆项
    float Gc = G_Smith(NoV, NoL, 0.25, isIBL);
    float Fc = 0.04 + 0.96 * Pow5(1 - Diff);
    float gtr1_alpha = 0.1 * (1.0 - clearCoatGloss) + 0.01 * clearCoatGloss;
    float Dc = (Pow2(gtr1_alpha) - 1.0) / (2 * PI * log(gtr1_alpha) * (Pow2(gtr1_alpha * NoH) + (1.0 - Pow2(NoH))));

    FclearCoat = clearCoat / 4.0 * vec3(Dc * Fc * Gc / (4 * NoL * NoV));


    return (Fdiffuse * (1.0 - subsurface) + Fsubsurface * subsurface) + Fsheen + FcookTorranceSpecular + FclearCoat;

    // test & Debug
    // return FclearCoat;
}