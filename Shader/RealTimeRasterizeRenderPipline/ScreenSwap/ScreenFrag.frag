#version 450 core

in vec2 scrCoord;

out vec4 FragColor;

uniform sampler2D screenRT;

void main(){
    vec3 ans = texture(screenRT, scrCoord).xyz;
    FragColor = vec4(ans.xyz, 1.0);
}