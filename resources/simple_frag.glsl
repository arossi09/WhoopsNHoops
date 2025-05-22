#version 330 core 

out vec4 color;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};
//probably going to get rid of this into struct format
uniform Material material;

//interpolated normal and light vector in camera space
in vec3 fragNor;
in vec3 lightDir;
//position of the vertex in camera space
in vec3 EPos;

void main()
{
	//you will need to work with these for lighting
	vec3 normal = normalize(fragNor);
	vec3 light = normalize(lightDir);
    vec3 viewDir = normalize(-EPos);

    //diffuse constant
    float dC = max(dot(normal, light), 0);
    
    //halfway vector
    vec3 H = normalize(lightDir + viewDir);

    vec3 diffuse = material.diffuse * dC;
    vec3 specular = material.specular * pow(max(dot(normal, H), 0.0), material.shininess);

	color = vec4(material.ambient+ diffuse + specular, 1);
}
