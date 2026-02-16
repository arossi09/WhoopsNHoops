#version 330

uniform sampler2D Texture0;

in vec2 TexCoord;
in vec3 fWorldPosition;
in vec3 fCameraPosition;
out vec4 FragColor;

void main() {
  vec3 fogColor = vec3(0.5, 0.6, 0.7);
  //vec3 fogColor = vec3(0.7, 0.7, 0.8);
  float fogDensity = 0.0025;
  vec3 fogOrigin = fCameraPosition;
  float fogDepth = distance(fWorldPosition, fogOrigin);
  float fogFactor = 1.0 - exp(-fogDensity * fogDensity * fogDepth * fogDepth);
  fogFactor = clamp(fogFactor, 0.0, 1.0);
  FragColor = texture(Texture0, TexCoord);

  if (FragColor.a < 0.1) {
    discard;
  }

  FragColor.rgb = mix(FragColor.rgb, fogColor, fogFactor);
}
