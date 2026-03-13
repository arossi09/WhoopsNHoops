#version 330 core
layout (location = 0) in vec2 vPos;
layout (location = 1) in vec2 vTex;

uniform mat4 M;
out vec2 vTexCoord;
void main(){
	gl_Position = M * vec4(vTex.xy, 0.0, 1.0);
	vTexCoord = vTex;
}
