#version  330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform int flip;
uniform int lightToggle;

out vec3 fragNor;
out vec2 vTexCoord;
out vec3 vWorldPosition;


void main() {
  vec4 clipCords = P * V * M * vec4(vertPos, 1.0);
  vec3 ndcPos = clipCords.xyz / clipCords.w;
  vec2 screenPos = (ndcPos.xy + 1.0f) * 0.5f;
  screenPos = floor(screenPos * vec2(320.0f, 240.0f));
  ndcPos.xy = (screenPos / vec2(320.0, 240.0)) * 2.0 - 1.0;

  gl_Position = P * V * M * vec4(vertPos.xyz, 1.0);
  //gl_Position = vec4(ndcPos.xy * clipCords.w, ndcPos.z * clipCords.w, clipCords.w);
  vWorldPosition = (M * vec4(vertPos.xyz, 1.0)).xyz;

  //lightDir = (V*(vec4(lightPos - wPos, 0.0))).xyz;
  fragNor = (M * vec4(vertNor, 0.0)).xyz;
  //lightDir = normalize((V * vec4(lightPos, 0.0)).xyz); for sun

  /* First model transforms */
  if (flip == 0) {
    fragNor = -fragNor;
  }

  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}
