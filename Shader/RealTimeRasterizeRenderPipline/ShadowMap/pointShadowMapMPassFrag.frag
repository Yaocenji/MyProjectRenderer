#version 450 core

in vec3 worldPos;
in float worldDepth;

uniform vec3 lightPos;
uniform float far;

void main()
{
    float lightDistance = length(worldPos.xyz - lightPos);
    // lightDistance = worldDepth;
    lightDistance = lightDistance / far;
    gl_FragDepth = lightDistance;
    // gl_FragDepth = 0.5;
}