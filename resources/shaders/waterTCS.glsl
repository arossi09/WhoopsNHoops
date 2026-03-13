#version 410 core
//we need to specify the number of control points per patch
//here we have 3 for a tri
layout(vertices = 3) out;

uniform mat4 model;
uniform mat4 view;

void main() {
  //pass the attributes through to the TES
  //here we just have the gl_Position
  gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;

  //invocation zero controlls tesselation level for the entire patch
  if (gl_InvocationID == 0) {
    //we need to define the constants to controll the tesselation parameters
    const int MIN_TESS_LEVEL = 4;
    const int MAX_TESS_LEVEL = 64;
    const float MIN_DISTANCE = 20;
    const float MAX_DISTANCE = 300;

    //we need to transform each vertex into eyespace
    vec4 eyeSpacePos0 = view * model * gl_in[0].gl_Position; //bottomn left vertex
    vec4 eyeSpacePos1 = view * model * gl_in[1].gl_Position; //top middle vertex
    vec4 eyeSpacePos2 = view * model * gl_in[2].gl_Position; //botoomn right vertex

    //we need to calculate the distance from camera relative to our distance range
    float distance0 = clamp((abs(eyeSpacePos0.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float distance1 = clamp((abs(eyeSpacePos1.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);
    float distance2 = clamp((abs(eyeSpacePos2.z) - MIN_DISTANCE) / (MAX_DISTANCE - MIN_DISTANCE), 0.0, 1.0);

    //Interpolate edge tesselation based off the closer vertex
    float tessLevel0 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance1, distance2)); //bottom right & bottom left
    float tessLevel1 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance2, distance0)); //bottom left & top middle
    float tessLevel2 = mix(MAX_TESS_LEVEL, MIN_TESS_LEVEL, min(distance0, distance1)); //top middel & bottom right

    //set corresponding outer edge teseselation levels
    gl_TessLevelOuter[0] = tessLevel0 ; //bottomn edgec
    gl_TessLevelOuter[1] = tessLevel1 ; //left edge
    gl_TessLevelOuter[2] = tessLevel2; //right edge

    //set the inner to the max of the two inner

    gl_TessLevelInner[0] = max(tessLevel0, max(tessLevel1, tessLevel2));
  }
}

