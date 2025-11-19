#version 330 core
layout (location = 0) in vec2 vertex;
layout (location = 1) in vec2 vTex;
uniform mat4 M;
uniform mat4 P;

out vec2 vTexCoord;
void main(){
    gl_Position = P * M * vec4(vertex.xy, 0.0, 1.0);
		vTexCoord = vTex;
}
