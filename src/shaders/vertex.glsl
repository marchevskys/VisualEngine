#version 430 core
#extension GL_ARB_separate_shader_objects : enable
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;

layout (location = 0)  uniform mat4 viewModel;
layout (location = 2) uniform mat4 projection;



out vec4 lp;
out vec4 cp;
out vec3 n;
out vec2 tc;


void main()
{
    lp = vec4(position, 1.0f);
    cp = viewModel * lp;
    n = mat3(viewModel) * normal;
    tc = texCoord;
    gl_Position  =  projection * cp ;

}
