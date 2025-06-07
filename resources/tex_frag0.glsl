#version 330 core
uniform sampler2D Texture0;
uniform int lightToggle;
uniform vec3 lightDirection;

in vec3 fragNor;
in vec2 vTexCoord;
out vec4 Outcolor;
void main() {
    vec4 texColor0 = texture(Texture0, vTexCoord);
    vec3 normal = normalize(fragNor);
	vec3 light = normalize(-lightDirection);
    float dC = max(dot(normal, light), .1);


  	//to set the out color as the texture color 
    if(lightToggle == 1){
        if(dC > .5){
            Outcolor = texColor0;
        }else{
            Outcolor = texColor0 * .5f;
        }
    }else{
        Outcolor = texColor0;
    }
  
  	//to set the outcolor as the texture coordinate (for debugging)
	//Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}


