#version 450 core

in vec2 scrCoord;

out vec4 FragColor;

uniform sampler2D screenRT;

// 是否进行hdr映射和gamma变换
uniform bool isDebug;

void main(){
    // 原始采样
    vec3 ans = texture(screenRT, scrCoord).xyz;

    if (!isDebug){
        // hdr到ldr
        ans /= (ans + vec3(1.0, 1.0, 1.0));
        // gamma矫正
        ans = pow(ans, vec3(2.2));
    }
    FragColor = vec4(ans.xyz, 1.0);
}