#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 M;
out vec3 fragNor;

void main(){
   
    fragNor = vertNor;
    gl_Position =  P * M * vec4(vertPos.xyz, 1.0f);

}
