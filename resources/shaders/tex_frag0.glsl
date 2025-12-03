#version 330 core
uniform sampler2D Texture0;
uniform int lightToggle;
uniform vec3 lightDirection;
uniform vec3 cameraPosition;

in vec3 fragNor;
in vec2 vTexCoord;
in vec3 vWorldPosition;
out vec4 Outcolor;
void main() {
  /*
    float fogDensity = 0.0015;
    vec3 fogColor = vec3(0.7, 0.7, 0.8);
    vec3 fogOrigin = cameraPosition;
    vec3 fogDirection = normalize(vWorldPosition - fogOrigin);
    float fogDepth = distance(vWorldPosition, fogOrigin);

    float heightFactor = 0.05;
    float fogFactor = heightFactor * exp(-fogOrigin.y * fogDensity) *
        (1.0 - exp(-fogDepth * fogDirection.y * fogDensity) / fogDirection.y);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
    float fogDensity = 0.0015;
    float fogFactor = 1.0 - exp(-fogDepth * fogDensity);
    fogFactor = clamp(fogFactor, 0.0, 1.0);
  	*/

  vec3 fogColor = vec3(0.7, 0.7, 0.8);
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
      Outcolor = vec4(texColor0.rgb * .9f, texColor0.a);
    } else {
      Outcolor = vec4(texColor0.rgb * .5f, texColor0.a);
    }
  } else {
    Outcolor = texColor0;
  }

  Outcolor.rgb = mix(Outcolor.rgb, fogColor, fogFactor);
}
