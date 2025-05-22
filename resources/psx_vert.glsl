#version  330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 V;
uniform mat4 M;


uniform vec3 lightPos;

out vec3 fragNor;
out vec3 lightDir;
out vec3 EPos;


void main()
{

    vec4 clipCords = P * V * M * vertPos;

    vec3 ndcPos = clipCords.xyz / clipCords.w;

    vec2 screenPos = (ndcPos.xy + 1.0f) *.5f;

    screenPos = floor(screenPos * vec2(320.0f, 240.0f));

    ndcPos.xy = (screenPos / vec2(320.0, 240.0)) * 2.0 - 1.0;


    gl_Position = vec4(ndcPos.xy * clipCords.w, ndcPos.z * clipCords.w, clipCords.w);
    fragNor = (M * vec4(vertNor, 0.0)).xyz;
    lightDir = lightPos - (M*vertPos).xyz;
	EPos = (M*vertPos).xyz;


}


