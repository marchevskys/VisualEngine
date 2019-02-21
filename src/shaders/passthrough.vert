#version 450 core
layout(location = 0) in vec3 position;
out vec2 UV;

void main(){
    gl_Position =  vec4(position * 0.3 + 0.7, 1.0);
    UV = (position.xy + vec2(1.0, 1.0)) / 2.0;
}

 
