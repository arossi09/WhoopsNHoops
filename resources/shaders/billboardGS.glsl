#version 330

layout(points) in;
layout(triangle_strip) out;
layout(max_vertices = 4) out;

uniform mat4 V;
uniform mat4 P;
uniform vec3 cameraPosition;
in vec3 vWorldPos[];

out vec2 TexCoord;

void main() {
  //extract position of point
  vec3 Pos = vWorldPos[0];
  vec3 cameraToPoint = normalize(Pos - cameraPosition);
  vec3 up = vec3(0.0, 1.0, 0.0);
  vec3 right = cross(up, cameraToPoint);

  //bottom left
  gl_Position = P * V * vec4(Pos, 1.0f);
	TexCoord = vec2(0.0, 0.0);
	EmitVertex();

	//top left
	Pos.y += 1.0;
	gl_Position = P * V * vec4(Pos, 1.0f);
	TexCoord = vec2(0.0, 1.0f);
	EmitVertex();

	//bottom right
	Pos.y -= 1.0;
	Pos+= right;
	gl_Position = P * V * vec4(Pos, 1.0);
	TexCoord = vec2(1.0, 0.0);
	EmitVertex();

	//top right
	Pos.y += 1.0;
	gl_Position = P * V * vec4(Pos, 1.0);
	TexCoord = vec2(1.0, 1.0);
	EmitVertex();

	EndPrimitive();
}
