#version 330 core
layout(location = 0) in vec3 vertPos;

out vec3 texCoords;
uniform mat4 V;
uniform mat4 P;
void main() {
  texCoords = vertPos;
  vec4 pos = P* V* vec4(vertPos, 1.0f);
  gl_Position = pos.xyww;
}
