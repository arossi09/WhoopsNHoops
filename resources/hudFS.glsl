#version 330 core
out vec4 color;
uniform sampler2D Texture0;

in vec2 vTexCoord;
uniform float uFilled;

void main(){
		float offset = vTexCoord.y > uFilled ? 0.0 : 0.5;
		vec2 texCoord = vec2(vTexCoord.x / 2 + offset, vTexCoord.y);
		vec4 texColor0 = texture(Texture0, texCoord);
    color = texColor0;
}

