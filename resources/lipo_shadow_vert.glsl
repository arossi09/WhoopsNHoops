#version 330 core
layout(location = 0) in vec3 vertPos;

uniform mat4 M;
uniform mat4 V;
uniform mat4 P;


void main(){
    gl_Position = P * V * M * vec4(vertPos, 1.0);
}
