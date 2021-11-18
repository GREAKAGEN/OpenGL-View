#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec3 norm;

uniform mat4 viewport;
uniform mat4 model;
uniform mat4 projection;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 viewPos;

out vec3 outColor;
//out vec3 normal;
//out vec3 worldPos;

vec3 computelight(vec3 worldPos,vec3 normal){
	vec3 lightdirection = normalize(lightPos - worldPos);  //light direction

	vec3 specdirect = normalize(- viewPos + worldPos); //specular--camera direction
	vec3 refdirect = reflect(-lightdirection, normal); //specular--reflect light
	if (dot(refdirect, normal) < 0)
		refdirect = vec3(0.0f, 0.0f, 0.0f);
	
	//ambient
	float amb = 0.4f;
	vec3 ambient = amb * lightColor;

	//diffuse
	float dif = max(dot(normal, lightdirection), 0.0f);
	vec3 diffuse = dif * lightColor;
	
	//specular
	float specstr = 0.15f;
	float spec = pow(max(dot(refdirect, specdirect),0.0f), 16);
	vec3 specular = specstr * spec * lightColor;

	return (ambient + diffuse + specular) * color;
}

void main(){
	gl_Position = projection * viewport * model * vec4(pos, 1.0f);
	vec3 normal = mat3 (transpose (inverse (model))) * norm;
	outColor = computelight(vec3(model * vec4(pos, 1.0f)),normal);
}