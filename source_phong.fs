#version 330 core

in vec3 outColor;
in vec3 normal;
in vec3 worldPos;

out vec4 fragcolor;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;
uniform vec4 ourColor;

//ambient, diffuse, specular
void main(){
	vec3 norm = normalize(normal);  //normal
	vec3 lightdirection = normalize(lightPos - worldPos);  //light direction

	vec3 specdirect = normalize(- viewPos + worldPos); //specular--camera direction
	vec3 refdirect = reflect(-lightdirection, norm); //specular--reflect light
	if (dot(refdirect, norm) < 0)
		refdirect = vec3(0.0f, 0.0f, 0.0f);

	
	//ambient
	float amb = 0.4f;
	vec3 ambient = amb * lightColor;

	//diffuse
	float dif = max (dot (norm, lightdirection), 0.0f);
	vec3 diffuse = dif * lightColor;
	
	//specular
	float specstr = 0.15f;
	float spec = pow(max(dot(refdirect, specdirect),0.0f), 16);
	vec3 specular = specstr * spec * lightColor;

	fragcolor =  vec4((ambient + diffuse + specular) * outColor, 1.0f);
}