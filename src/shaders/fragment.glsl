#version 430 core
#extension GL_ARB_separate_shader_objects : enable

#define pi 3.1415926536
#define pi2 1.57079632679
#define pi3 1.0471975512
layout(origin_upper_left) in vec4 gl_FragCoord;
out vec4 FragColor;



uniform float time;
uniform sampler2D diffuseMap;
//uniform sampler2D reflectionMap;

in vec4 lp;
in vec4 cp;
in vec3 n;
in vec2 tc;


void main()
{

    vec3 nn = normalize(n);
    //nn = normalize(cross(dFdx(cp.xyz), dFdy(cp.xyz)));

    vec3 dir = -normalize(cp.xyz);
//dir = vec3(1,0,0);

    //float angle = atan(nn.y / nn.x) / (pi * 2) + 0.2;

float f = dot(dir, nn);
vec4 color = texture2D(diffuseMap, tc);
    FragColor = vec4(color.rgb * f + vec3(pow(f, 80)), 1);

}
