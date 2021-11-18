#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;
layout(location=2) in vec3 norm;

uniform mat4 viewport;
uniform mat4 model;
uniform mat4 projection;

out vec3 outColor;
out vec3 normal;
out vec3 worldPos;

void main(){
	gl_Position = projection * viewport * model * vec4(pos, 1.0f);
	outColor = color;
	normal = mat3(transpose(inverse(model))) * norm;
	worldPos = vec3(model * vec4(pos, 1.0f));
}