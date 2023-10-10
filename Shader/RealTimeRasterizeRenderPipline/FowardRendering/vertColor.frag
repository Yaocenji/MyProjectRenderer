#version 450 core

in vec4 ndcPos;
in vec3 norm;
in vec3 tang;
in vec4 col;

in vec2 uv0;

out vec4 FragColor;

void main(){
    FragColor = vec4(col.xyz, 1.0);

    // TEST
    // FragColor = vec4(ndcPos.yyy, 1);
    // FragColor = vec4(uv0.xy, 1, 1);
}