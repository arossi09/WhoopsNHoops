#version 330 core
uniform sampler2D Texture0;
in vec2 vTexCoord;
out vec4 color;
void main(){
	vec4 texColor = texture(Texture0, vTexCoord);
	color = texColor;
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}


