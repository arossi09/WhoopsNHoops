
#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 V;
uniform mat4 P;
uniform mat4 M;
uniform vec3 cameraPosition;
in vec3 vModelPos[];

out vec2 TexCoord;
out vec3 fCameraPosition;
out vec3 fWorldPosition;

void main() {
  //extract position of point
  vec3 Pos = vModelPos[0];
  vec3 cameraToPoint = normalize(Pos - cameraPosition);
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = normalize(cross(up, cameraToPoint));
  vec3 billboardUp = vec3(0.0, 1.0, 0.0);

	
  float treeWidth = 0.05;
  float treeHeight = 0.1;

  // bottom-left
  gl_Position = P * V * M * vec4(Pos - right * treeWidth, 1.0);
  TexCoord = vec2(0.0, 0.0);
	fCameraPosition = cameraPosition;
	fWorldPosition =  vec3(M * vec4(Pos, 1.0));
  EmitVertex();

  // top-left
  gl_Position = P * V * M * vec4(Pos - right * treeWidth + billboardUp * treeHeight, 1.0);
  TexCoord = vec2(0.0, 1.0);
	fCameraPosition = cameraPosition;
	fWorldPosition =  vec3(M * vec4(Pos, 1.0));
  EmitVertex();

  // bottom-right
  gl_Position = P * V * M * vec4(Pos + right * treeWidth, 1.0);
  TexCoord = vec2(1.0, 0.0);
	fWorldPosition =  vec3(M * vec4(Pos, 1.0));
	
	fCameraPosition = cameraPosition;
  EmitVertex();

  // top-right
  gl_Position = P * V * M * vec4(Pos + right * treeWidth + billboardUp * treeHeight, 1.0);
  TexCoord = vec2(1.0, 1.0);
	fCameraPosition = cameraPosition;
	fWorldPosition =  vec3(M * vec4(Pos, 1.0));
  EmitVertex();

  EndPrimitive();

}
