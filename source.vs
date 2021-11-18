#version 330 core
layout(location=0) in vec3 pos;
layout(location=1) in vec3 color;

uniform mat4 viewport;
uniform mat4 model;
uniform mat4 projection;

out vec3 outColor;

void main(){
	gl_Position = projection * viewport * model * vec4(pos, 1.0f);
	outColor = color;
}