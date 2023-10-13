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

/*

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
*/

// normal-ao
uniform sampler2D normalAO;
uniform vec4 normalAOVec;
uniform bool normalAOUseTex;

/*
// Luminance
uniform sampler2D Luminance;
uniform vec4 LuminanceVec;
uniform bool LuminanceUseTex;
// Transparency
uniform sampler2D Transparency;
uniform vec4 TransparencyVec;
uniform bool TransparencyUseTex;

*/

uniform mat4 model_inverse;
// 摄像机参数
uniform mat4 view_inverse;
uniform mat4 proj_inverse;

uniform vec3 cameraPos;

uniform float near;
uniform float far;

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

void main(){
    vec3 worldPos = rebuildWorldPos();

    vec3 albedo;
    vec4 detailNormalAO;
    float ao;
    //首先读取系列纹理
    if (diffuseOrAlbedoUseTex)
        albedo = texture(diffuseOrAlbedo, uv0).xyz;
    else
        albedo = diffuseOrAlbedoVec.xyz;
    
    if (normalAOUseTex)
        detailNormalAO = texture(normalAO, uv0);
    else
        detailNormalAO = normalAOVec;

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
    // ao变换
    // albedo *= detailNormalAO.w;

    vec3 ans = vec3(0);
    // 首先计算主光源平行光
    if (useMainLight) {
        ans += max(0, dot(mLight.Direction, worldDetailedNorm)) * mLight.ColorAndStrength.xyz * mLight.ColorAndStrength.w * albedo.xyz;
    }
    // 点光源
    for(int i = 0; i < min(usedPointLightNumber, POINT_LIGHT_MAX_NUMBER); ++i){
        float dist = length(pLight[i].Position - worldPos.xyz);
        vec3 Direction = normalize(pLight[i].Position - worldPos.xyz);

        ans += max(0, dot(Direction, worldDetailedNorm)) * pLight[i].ColorAndStrength.xyz * pLight[i].ColorAndStrength.w * exp(-dist) *albedo.xyz;
    }

    FragColor = vec4(ans.xyz, 1.0);

    // testAndDebug
    // float a = length(worldNorm - worldDetailedNorm);
    // FragColor = vec4(worldDetailedNorm * 0.5 + 0.5, 1.0);
    // FragColor = vec4(worldNorm, 1.0);
    // FragColor = vec4(AO.xxx, 1);
}