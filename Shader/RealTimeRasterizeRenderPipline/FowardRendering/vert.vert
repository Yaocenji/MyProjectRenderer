#version 450 core

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec3 Tangent;
layout (location = 3) in vec4 Color;

layout (location = 4) in vec2 Texcoord0;
// layout (location = 5) in vec2 Texcoord1;
// layout (location = 6) in vec2 Texcoord2;
// layout (location = 7) in vec2 Texcoord3;
// layout (location = 8) in vec2 Texcoord4;
// layout (location = 9) in vec2 Texcoord5;
// layout (location = 10) in vec2 Texcoord6;
// layout (location = 11) in vec2 Texcoord7;

out vec4 worldPosition;
out float worldDepth;
out vec4 clipPos;
out vec4 ndcPos;
out vec3 tang;
out vec3 worldNorm;
out vec4 col;

out vec2 uv0;


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

uniform mat4 model_inverse;
uniform mat4 model_inverse_transpose;
// uniform mat4 view_inverse;
// uniform mat4 proj_inverse;

uniform vec3 cameraPos;


void main(){
    // TODO：此处的变换特别考虑
    worldNorm = normalize((model_inverse_transpose * vec4(Normal, 0.0)).xyz);
    tang = Tangent;

    col = Color;

    uv0 = Texcoord0;
    worldPosition = model * vec4(Position, 1.0);
    worldDepth = length(cameraPos - worldPosition.xyz);
    clipPos = proj * view * worldPosition;
    ndcPos = clipPos / clipPos.w;
    gl_Position = ndcPos;

    // TEST
    // gl_Position = vec4(Position, 1.0);
}