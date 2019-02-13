#version 420 core

in vec2 UV;
out vec3 color;

uniform sampler2D tex1;
//uniform sampler2D depthTexture;
//uniform float time;



void main(){

    float col = fract(
               texture2D(tex1, UV).r
//                texture(tex1, vec3(UV, 9999))
                //texture(tex1, vec3(UV, 1.0)).r
                      * 1000.4);
    color = vec3(col);
        


}
