#version 330 core 


uniform vec3 baseColor;
out vec4 color;

in vec3 normal_cam;
in vec3 light_dir_cam;
out vec4 fragColor;



void main() {
    float intensity = max(dot(normalize(normal_cam), normalize(light_dir_cam)), 0.0);
    float final = 1;

    if (intensity > 0.95)
		final= 1;
	else if (intensity > 0.5)
		final = .8;
	else if (intensity > 0.25)
		final= .5;
	else
		final= .2;


    fragColor = vec4(baseColor*final, 1.0);
}
