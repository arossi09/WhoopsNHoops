#version 330 core

struct Material {
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
  float shininess;
};

struct DirLight {
  vec3 direction;
  vec3 ambient;
  vec3 diffuse;
  vec3 specular;
};

out vec4 FragColor;

in vec3 Normal;
in vec3 fragPos;

float uFresnalNormalStrength = 1.0; 
float uFresnalShininess = 3.65;
float uFresnalBias = 0.182; 
float uFresnalStrength = 0.7;
float utipAttentuation =6.5;

uniform DirLight dirLight;
uniform mat4 model;
uniform Material material;
uniform vec3 viewPos;
uniform samplerCube skybox;

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir);

float getFoamLerp(float h, float maxH, vec2 foamRange, float heightFoamAmt,
  float heightFoamFalloff, vec3 n, float angleFoamAmt, float angleFoamFalloff) {
  float hRatio = clamp(h / maxH, 0.0, 1.0);
  hRatio = (hRatio - foamRange.x) / (foamRange.y - foamRange.x);
  hRatio = clamp((hRatio - 1 + heightFoamAmt) / heightFoamAmt, 0.0, 1.0);
  hRatio = pow(hRatio, heightFoamFalloff);

  float angleFoam = clamp(n.y, 0, 1);
  angleFoam = clamp((angleFoam - 1 + angleFoamAmt) / angleFoamAmt, 0.0, 1.0);
  angleFoam = pow(angleFoam, angleFoamFalloff);

  return mix(hRatio, angleFoam, hRatio);
}

void main()
{
  float height = fragPos.y;
  //calculate the normal of the vertices
  vec3 norm = normalize(Normal);
  vec3 viewDir = normalize(viewPos - fragPos);
  //we need the vertex from the frag position to the camera

  //calculate schlick fresnel
  vec3 fresnalNormal = norm;
  fresnalNormal.xz *= uFresnalNormalStrength;
  fresnalNormal = normalize(fresnalNormal);
  float base = 1 - max(dot(viewDir, fresnalNormal), 0.0);
  float exponential = pow(base, uFresnalShininess);
  float R = exponential + uFresnalBias * (1.0 - exponential);
  R *= uFresnalStrength;

  //calculate and sample reflection from skybox
  vec3 reflectedDir = reflect(-viewDir, norm);
  vec3 reflection = texture(skybox, reflectedDir).rgb;
  vec3 fresnel = reflection.rgb * R;
	float modelScaleY = length(model[1].xyz);
  
  // Extract the model's Y translation (position of the base plane)
  float modelTranslateY = model[3].y;
  
  // Use relative height from the base plane
  float relativeHeight = fragPos.y - modelTranslateY;
  float normalizedHeight = clamp(relativeHeight / modelScaleY, 0.0, 2.0);
  //add color attentuation to tips based of height
	//float modelScaleY = length(model[1].xyz); 
	//float normalizedHeight = clamp(height / modelScaleY, 0.0, 2.0);
	vec3 uTipColor = vec3(0.8, 0.85, 0.9);
  vec3 tipColor = uTipColor * pow(normalizedHeight, utipAttentuation);



  vec3 lighting = calcDirLight(dirLight, norm, viewDir);
  vec3 finalColor = lighting + tipColor + fresnel;
  //vec3 finalColor = mix(baseColor, reflection, R); // blends reflection smoothly
  
  vec3 fogColor = vec3(0.5, 0.6, 0.7);
  float fogDensity = 0.0025;
  vec3 fogOrigin = viewPos;
  float fogDepth = distance(fragPos, fogOrigin);
  float fogFactor = 1.0 - exp(-fogDensity * fogDensity * fogDepth * fogDepth);
  fogFactor = clamp(fogFactor, 0.0, 1.0);

	//FragColor = vec4(finalColor, 1.0);
  FragColor = vec4(lighting + fresnel + tipColor, 1.0);

  FragColor.rgb = mix(FragColor.rgb, fogColor, fogFactor);
}

vec3 calcDirLight(DirLight light, vec3 normal, vec3 viewDir) {
  vec3 lightDir = -normalize(light.direction);
  //ambient
  vec3 ambient = light.ambient * material.ambient;

  //calculate the angle of the light hitting the vertex
  float ndotl = max(dot(normal, lightDir), 0);
  vec3 diffuse = light.diffuse * ndotl * (material.diffuse * 1.5);
  vec3 halfway = normalize(lightDir + viewDir);

  //schlick fresnel
  float spec = pow(max(dot(halfway, normal), 0.0), material.shininess)*ndotl ;
  vec3 specular = light.specular.rgb * (spec * material.specular) ;


	float f0 = 0.38;   // Water reflectance at normal incidence
	float hv = clamp(dot(viewDir, normal), 0.0, 1.0);
	float fresnelSpec = f0 + (1.0 - f0) * pow(1.0 - hv, 5.0);
	specular *= fresnelSpec;

	/*
  float base = 1 - max(dot(viewDir, halfway), 0.0);
  float exponential = pow(base, 5.0f);
  float R = exponential + uFresnalBias * (1.0f - exponential);
  specular *= R;
	*/

  return (ambient + diffuse /*+ specular*/);
}

