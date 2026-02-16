#version 330
layout (location = 0) in vec3 vertPos;

out vec3 vModelPos;
void main(){
	vModelPos = vertPos;
}
