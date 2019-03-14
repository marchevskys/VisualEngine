#version 450 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

uniform vec3 diffuseColor;

in VS_OUT {
    vec4 wp;    // world position
    vec4 cp;    // camera position
    vec4 lp;    // local position
    vec3 n;     // normal
    vec2 tc;    // texture coordinate
} vs;

layout(location = 0) out vec3 color;


void main(){
    color = diffuseColor;
}
