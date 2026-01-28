#version 330 core
uniform sampler2D Texture0;
uniform int lightToggle;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;

in vec3 fragNor;
in vec2 vTexCoord;
in vec3 vWorldPosition;
out vec4 Outcolor;

vec3 applyFog(vec3 col, float t, vec3 ro, vec3 rd) {
  float a = 0.0025;
  float b = 0.1;
  rd = normalize(rd);
  float rdy = abs(rd.y) < 0.0001 ? 0.0001 : rd.y;
  float fogAmount = (a / b) * exp(-ro.y * b) * (1.0 - exp(-t * rd.y * b)) / rdy;
  vec3 fogColor = vec3(0.5, 0.6, 0.7);
  return mix(col, fogColor, fogAmount);
}
void main() {

  vec3 fogColor = vec3(0.5, 0.6, 0.7);
  //vec3 fogColor = vec3(0.7, 0.7, 0.8);
  float fogDensity = 0.0025;
  vec3 fogOrigin = cameraPosition;
  float fogDepth = distance(vWorldPosition, fogOrigin);
  float fogFactor = 1.0 - exp(-fogDensity * fogDensity * fogDepth * fogDepth);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

  vec4 texColor0 = texture(Texture0, vTexCoord);
  vec3 normal = normalize(fragNor);
  vec3 light = normalize(-lightDirection);
  float dC = max(dot(normal, light), .1);

  //to set the out color as the texture color
  if (lightToggle == 1) {
    if (dC > .5) {
      Outcolor = vec4(texColor0.rgb * .8f, texColor0.a);
    } else {
      Outcolor = vec4(texColor0.rgb * .45f, texColor0.a);
    }
  } else {
    Outcolor = texColor0;
  }

  Outcolor.rgb = mix(Outcolor.rgb, fogColor, fogFactor);
}
