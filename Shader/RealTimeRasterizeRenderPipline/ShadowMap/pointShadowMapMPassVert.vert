#version 450 core
layout (location = 0) in vec3 position;

out vec3 worldPos;
out float worldDepth;

uniform mat4 model;
uniform mat4 shadowMatrices[6];

uniform int pass;

uniform vec3 lightPos;

void main()
{
    worldPos = (model * vec4(position, 1.0)).xyz;
    worldDepth = length(worldPos.xyz - lightPos);
    gl_Position = shadowMatrices[pass] * model * vec4(position, 1.0);
}