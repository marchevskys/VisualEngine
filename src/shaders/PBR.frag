#version 430 core
#extension GL_ARB_separate_shader_objects : enable

#define pi 3.1415926536
#define pi2 1.57079632679
#define pi3 1.0471975512
out vec4 FragColor;
layout(origin_upper_left) in vec4 gl_FragCoord;

layout (location = 0) uniform mat4 model;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 projection;
layout (location = 3) uniform vec3 cameraPos;
layout (location = 4) uniform mat4 lightSpaceMatrix;

layout (location = 10) uniform vec3 diffuseColor;

uniform sampler2D shadowMap;


in VS_OUT {
    vec4 wp;
    vec4 lp;
    vec3 n;
    vec2 tc;
    vec4 lsPos;
} vs;


void main()
{

    vec3 nn = normalize(vs.n);
    //nn = normalize(cross(dFdx(cp.xyz), dFdy(cp.xyz)));

    vec3 dir = normalize(cameraPos - vs.wp.xyz);
    vec3 light = vec3(0, 0, 1);

    //float angle = atan(nn.y / nn.x) / (pi * 2) + 0.2;

    float diff = clamp(dot(light, nn), 0, 1);
    float refl = clamp(dot(-reflect(dir, nn), light),0,1);

    //vec4 texColor = texture2D(diffuseMap, vs.tc);
    vec4 shadowDepth = texture2D(shadowMap, vs.tc);
    vec3 finalColor = /*texColor.rgb +*/ diffuseColor;
    FragColor = vec4(finalColor * diff + vec3(pow(refl, 80)), 1);

}
