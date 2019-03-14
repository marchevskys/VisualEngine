#version 450 core
#extension GL_EXT_texture_array : enable
#extension GL_ARB_conservative_depth : enable
#define PI 3.1415926535
#define MAX_SHADOW_MAP_CASCADE_COUNT 16

//#define DEBUG
layout(early_fragment_tests) in;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
#ifdef DEBUG
uniform sampler2DArray shadowMap;
#else
uniform sampler2DArrayShadow shadowMap;
#endif
uniform vec3 lightDir;
uniform vec3 viewPos;

uniform int shadowCascadeCout;
uniform float cascadeSplits[MAX_SHADOW_MAP_CASCADE_COUNT];
uniform mat4 cascadeTransforms[MAX_SHADOW_MAP_CASCADE_COUNT];

uniform vec3 diffuseColor;

in VS_OUT {
    vec4 wp;    // world position
    vec4 cp;    // camera position
    vec4 lp;    // local position
    vec3 n;     // normal
    vec2 tc;    // texture coordinate
} vs;

layout(location = 0) out vec3 color;

vec2 poissonDisk[16] = vec2[](
   vec2(-0.94201624, -0.39906216), vec2(0.94558609, -0.76890725), vec2(-0.094184101, -0.92938870), vec2(0.34495938, 0.29387760),
   vec2(-0.91588581, 0.45771432), vec2(-0.81544232, -0.87912464), vec2(-0.38277543, 0.27676845), vec2(0.97484398, 0.75648379),
   vec2(0.44323325, -0.97511554), vec2(0.53742981, -0.47373420), vec2(-0.26496911, -0.41893023), vec2(0.79197514, 0.19090188),
   vec2(-0.24188840, 0.99706507), vec2(-0.81409955, 0.91437590), vec2(0.19984126, 0.78641367), vec2(0.14383161, -0.14100790)
);

float fresnelSchlick(float cosTheta, float f)
{
    return f + (1.0 - f) * pow(1.0 - cosTheta, 5.0);
}

vec3 rainbow(float v)
{
    vec3 color = vec3(sin(v), sin(v + 3.1415926535 / 3), sin(v + 2 * 3.1415926535 / 3));
    return color * color;
}

float rand(vec3 coord){
    return fract(sin(dot(coord, vec3(12.9898,78.233, 37.7464))) * 43758.5453) * 2 - 1;
}

const mat4 biasMat = mat4(0.5, 0.0, 0.0, 0.0, 0.0, 0.5, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.5, 0.5, 0.0, 1.0);


void main(){
    vec3 nn = normalize(vs.n);


    vec3 viewDir = normalize(viewPos - vs.wp.xyz);

    vec3 lDir = lightDir.rgb;

    float lightDot = clamp(dot(lDir, nn), 0, 1);
    float viewDot = abs(dot(viewDir, nn));
    float spec = -dot(reflect(viewDir, nn), lDir);
    vec3 skyDir = vec3(0, 0, 1);

    vec3 finalColor = diffuseColor;

    float zPosition = vs.cp.z;
    int cascadeIndex = 0;
    for(int i = 0; i < shadowCascadeCout - 1; ++i)
        if(zPosition < cascadeSplits[i])
            cascadeIndex = i + 1;

    vec4 shadowCoord = (biasMat * cascadeTransforms[cascadeIndex]) * vs.wp;
    ////////////////////////////////////////////////////////////////
//    vec3 colorArray[4] = vec3[](vec3(1,0,0), vec3(0,1,0), vec3(0,0,1), vec3(0,0,0));
//    color = vec3(0);
//    for(int i = 0; i < shadowCascadeCout; i++){
//        vec4 shadowCoord = (biasMat * cascadeTransforms[i]) * vs.wp;
//        float pattern = fract((gl_FragCoord.x - gl_FragCoord.y) * 0.1) > 0.1 ? 0.0 : 0.24;
//        if(shadowCoord.x > 0 && shadowCoord.x < 1 && shadowCoord.y > 0 && shadowCoord.y < 1){
//            vec2 c = 2.0 * (shadowCoord.xy - 0.5);
//            c = pow(c, vec2(2));

//            if(i == cascadeIndex)
//                color += colorArray[i] * (max(c.x, c.y)) * texture(shadowMap, vec3(shadowCoord.xy, i)).r;
//            else
//                color += colorArray[i] * (max(c.x, c.y)) * texture(shadowMap, vec3(shadowCoord.xy, i)).r * pattern;
//        }
//    }
//    return;
    ///////////////////////////////////////////////////////////////


    float shadow = 0.0;
    int discSize = 16;
    for(int i = 0 ; i < discSize; i++){
        vec2 pd = (1.0 / 2048.0) * poissonDisk[i] * (gl_FragCoord.w * 3 + 1);
#ifdef DEBUG
        shadow += texture(shadowMap, vec3(shadowCoord.xy + pd, cascadeIndex)).r - 0.5 - shadowCoord.z * 0.5 < 0 ? 0.0 : 1.0;
#else
        shadow += texture(shadowMap, vec4(shadowCoord.xy + pd, cascadeIndex, 0.5 + shadowCoord.z * 0.5));
#endif

    }
    shadow /= discSize;

    float skyReflection = dot(reflect(viewDir, nn), -skyDir);
    float skyDot = -dot(skyDir, nn);

    float fresnel = fresnelSchlick(viewDot, 0.04);
    float lightness =  shadow * lightDot;


    vec3 diffuse = lightness * finalColor;
    vec3 specular = 0.01 * lightness * vec3(1/(1-clamp(spec, 0,1))) * fresnel;
    float ambMultiplier = (1 - viewDot) * (1 - lightness) * 0 + pow(0.5 - skyDot * 0.5, 3);
    vec3 ambient = finalColor * (ambMultiplier * 0.1 + 0.1);
    vec3 skyColor = fract(clamp(1 - skyReflection, -0.5, 1.0)) * fresnel * vec3(0.7, 0.7, 1.0);
    color = diffuse + specular + ambient + skyColor;



    color = color / (color + vec3(1.0));
    color = vec3(1) - pow(vec3(1) - color, vec3(4));

#ifdef DEBUG
    //float mask = vs.cp.x < 0.4 || fract((gl_FragCoord.x - gl_FragCoord.y) * 0.03) > 0.9 ? 0 : 0.4;
    //color = mix(color, rainbow(cascadeIndex * 0.3), mask);
#endif
}
