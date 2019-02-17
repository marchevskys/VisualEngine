#version 420 core

in vec2 UV;
out vec3 color;

uniform sampler2D tex1;
//uniform sampler2D depthTexture;
//uniform float time;



void main(){

    float col = (texture2D(tex1, UV).r);

    color = vec3(col);
        


}
