#version 450 core

layout(location = 0) in vec3 scrVertPos;
// layout(location = 2) in vec3 scrNorm;

out vec2 scrCoord;

void main(){
    scrCoord = scrVertPos.xy * 0.5 + 0.5;
    gl_Position = vec4(scrVertPos.xy, 0.0, 1.0);
}