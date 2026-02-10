#version 330
layout (location = 0) in vec3 vertPos;

uniform mat4 M;
out vec3 vWorldPos;
void main(){
	vec4 w = M * vec4(vertPos, 1.0f);
	vWorldPos = w.xyz;
}
