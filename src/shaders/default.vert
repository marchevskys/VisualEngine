#version 430 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform vec3 cameraPos;
layout (location = 4) uniform mat4 lightSpaceMatrix;

out VS_OUT {
    vec4 wp;
    vec4 lp;
    vec3 n;
    vec2 tc;
    vec4 lsPos;
} vs;

void main()
{
    vs.lp = vec4(position, 1.0f);
    vs.wp = model * vs.lp;
    gl_Position  =  projection * view * vs.wp ;
    vs.n = mat3(model) * normal;
    vs.lsPos = lightSpaceMatrix * vs.wp;
    vs.tc = texCoord;

}
