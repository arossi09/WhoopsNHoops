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

void main() {

  gl_Position = P * V *M * vec4(vertPos.xyz, 1.0);

  //lightDir = (V*(vec4(lightPos - wPos, 0.0))).xyz;
  fragNor = (M * vec4(vertNor, 0.0)).xyz;
  //lightDir = normalize((V * vec4(lightPos, 0.0)).xyz); for sun

  /* First model transforms */
  if(flip == 0){
        fragNor = -fragNor;
  }

  
  /* pass through the texture coordinates to be interpolated */
  vTexCoord = vertTex;
}

