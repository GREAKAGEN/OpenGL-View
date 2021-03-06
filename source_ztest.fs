#version 330 core

in vec3 outColor;

out vec4 fragcolor;

uniform vec4 ourColor;

float near = 0.1f;
float far = 3.5f;

float LinearizeDepth(float depth)
{
	float z = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main(){
	float depth = LinearizeDepth(gl_FragCoord.z) / far;
	fragcolor = vec4(vec3(depth), 1.0f);
}