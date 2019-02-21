#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 shadowMatrix;


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;


out VS_OUT {
    vec4 wp;    // world position
    vec4 cp;    // camera position
    vec4 lp;    // local position
    vec4 shadowCoord;   // shadow coordinate
    vec3 n;     // normal
    vec2 tc;    // texture coordinate
} vs;

void main(){

        vs.lp = vec4(vertexPosition, 1.0f);
        vs.wp = model * vs.lp;
        vs.cp = view * vs.wp;
        gl_Position  =  projection * vs.cp;
        vs.shadowCoord = shadowMatrix * vs.wp;
        vs.n = mat3(model) * vertexNormal;
        vs.tc = vertexUV;
}
