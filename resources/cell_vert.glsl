#version 330 core 
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;

out vec3 normal_cam;
out vec3 light_dir_cam;

void main() {
    // Transformations...
    normal_cam = normalize(mat3(V * M) * vertNor);
    vec3 lightPos_cam = vec3(V * vec4(lightPos, 1.0));
    light_dir_cam = normalize(lightPos_cam - vec3(V * M * vec4(vertPos, 1.0)));
    gl_Position = P * V * M * vec4(vertPos, 1.0);


}
