#version 330 core

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 shadowMatrix;


layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec3 vertexNormal;
layout(location = 2) in vec2 vertexUV;


out VS_OUT {
    vec4 wp;
    vec4 lp;
    vec4 shadowCoord;
    vec3 n;
    vec2 tc;
} vs;

void main(){

        vs.lp = vec4(vertexPosition, 1.0f);
        vs.wp = model * vs.lp;
        gl_Position  =  projection * view * vs.wp;
        vs.shadowCoord = shadowMatrix * vs.wp;
        vs.n = mat3(model) * vertexNormal;
        vs.tc = vertexUV;
}
