#version 330

uniform sampler2D Texture0;

in vec2 TexCoord;
out vec4 FragColor;

void main() {
  FragColor = texture(Texture0, TexCoord);

  if (FragColor.r == 0 && FragColor.g == 0 && FragColor.b == 0) {
    discard;
	}
}
