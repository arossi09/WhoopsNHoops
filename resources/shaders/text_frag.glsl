#version 330 core

in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;
uniform vec2 uvOffset;
uniform vec2 uvSize;

void main(){
    float alpha = texture(text, uvOffset + TexCoords * uvSize).r;
    color = vec4(textColor, alpha);
}
