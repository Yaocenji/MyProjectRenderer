#version 450 core

in vec2 scrCoord;

out vec4 FragColor;

void main(){
    FragColor = vec4(scrCoord.xy, 0.0, 1.0);
}