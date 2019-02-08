#version 330 core
layout(location = 0) in vec3 vertexPosition;
//layout(location = 1) in vec3 vertexNormal;
//layout(location = 2) in vec2 vertexUV;

uniform mat4 viewProjection;
uniform mat4 model;

void main()
{
    gl_Position = viewProjection * model * vec4(vertexPosition, 1.0);
}  
