#version 410 core

layout(triangles, fractional_odd_spacing, ccw) in;

uniform float g_Time;
float u_Seed = 4; // Starting seed
float u_SeedIter = 4.3; // Increment per wave
float u_Frequency = 1; // Base frequency
float u_FrequencyMult = 1.15; // Multiplier per iteration
float u_Amplitude = 1; // Base amplitude
float u_AmplitudeMult = 0.83; // Multiplier per iteration
float u_InitialSpeed = 1; // Base speed
float u_SpeedRamp = 1.0; // Speed multiplier per iteration
float u_MaxPeak = 1; // For exp function 
float u_PeakOffset = 1.14; // For exp function 
float u_vertexDrag = 0.5; // For exp function
int u_NumWaves = 32; // Number of iterations
float u_vertexHeight = 1.48; // Number of iterations

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 Normal;
out vec3 fragPos;

float waveHeight(vec3 pos, out vec3 normal ) {
  float totalHeight = 0.0;
  float dx = 0.0;
  float dz = 0.0;

  float f = u_Frequency;
  float a = u_Amplitude;
  float speed = u_InitialSpeed;
  float seed = u_Seed;
  float amplitudeSum = 0.0;

  for (int i = 0; i < u_NumWaves; i++) {
    vec2 dir = normalize(vec2(cos(seed), sin(seed)));

    float projection = dot(dir, pos.xz);
    float theta = projection * f + g_Time * speed;

    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    float expTerm = exp(u_MaxPeak * sinTheta - u_PeakOffset);
    float wave = a * expTerm;

    totalHeight += wave;

    // Derivative for normals
    float derivativeFactor = f * u_MaxPeak * wave * cos(theta);
    dx += derivativeFactor * dir.x;
    dz += derivativeFactor * dir.y;

    //domain warping
    pos.xz += -vec2(derivativeFactor * dir.x, derivativeFactor * dir.y) * a * u_vertexDrag;

    amplitudeSum += a;
    // Scale for next iteration
    f *= u_FrequencyMult;
    a *= u_AmplitudeMult;
    speed *= u_SpeedRamp;
    seed += u_SeedIter; // New random direction next iteration
  }

  // Normalize by total amplitude
  totalHeight /= amplitudeSum;
  dx /= amplitudeSum;
  dz /= amplitudeSum;

  //vec3 tangent = normalize(vec3(1.0, dx, 0.0));
  //vec3 binormal = normalize(vec3(0.0, dz, 1.0));
  //normal = normalize(cross(binormal, tangent));
	normal = normalize(vec3(-dx, 1.0, -dz));


  totalHeight *= u_vertexHeight;


	return totalHeight;
}


void main() {

	vec3 b = gl_TessCoord.xyz;
  vec3 waveNormal;
  vec3 pos = b.x * gl_in[0].gl_Position.xyz + b.y * gl_in[1].gl_Position.xyz + b.z * gl_in[2].gl_Position.xyz;

	/*
	float modelScale = length(model[0].xyz);
  float scaleInverse = 1.0 / modelScale;

  vec4 modelPos = model * vec4(pos, 1.0f);
  modelPos.y += waveHeight(modelPos.xyz, waveNormal);
  gl_Position = projection * view * modelPos;
	*/

	pos.y += waveHeight(pos, waveNormal);
  gl_Position = projection * view * model * vec4(pos, 1.0f);

  fragPos = (model * vec4(pos, 1.0f)).xyz;
  Normal = mat3(transpose(inverse(model))) * waveNormal;

}

