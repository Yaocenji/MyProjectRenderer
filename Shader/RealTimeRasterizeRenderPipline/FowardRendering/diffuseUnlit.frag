#version 450 core

in vec4 ndcPos;
in vec3 tang;
in vec4 col;

in vec2 uv0;

out vec4 FragColor;

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
uniform sampler2D sheenAndTintWithclearCoatAndTint;
uniform vec4 sheenAndTintWithclearCoatAndTintVec;
uniform bool sheenAndTintWithclearCoatAndTintUseTex;
// normal-ao
uniform sampler2D normalAO;
uniform vec4 normalAOVec;
uniform bool normalAOUseTex;

*/


void main(){
    vec4 albedo;
    if (diffuseOrAlbedoUseTex)
        albedo = texture(diffuseOrAlbedo, uv0);
    else
        albedo = diffuseOrAlbedoVec;

    FragColor = vec4(albedo.xyz, 1.0);

}