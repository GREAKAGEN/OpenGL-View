#pragma once
#ifndef BUFFER_PROM
#define BUFFER_PROM
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;

int indexcount;

const char* vertexShaderSource = "#version 330 core\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec3 color;\n"
"uniform mat4 viewport;\n"
"out vec3 outColor;\n"
"void main(){\n"
"	gl_Position = viewport * vec4(pos, 1.0f);\n"
"	outColor = color;\n"
"}\n\0";

const char* vertexShaderSourceAdd = "#version 330 core\n"
"layout(location=0) in vec3 pos;\n"
"layout(location=1) in vec3 color;\n"
"uniform mat4 viewport;\n"
"uniform mat4 model;\n"
"uniform mat4 projection;\n"
"out vec3 outColor;\n"
"void main(){\n"
"	gl_Position = projection * viewport * model * vec4(pos, 1.0f);\n"
"	outColor = color;\n"
"}\n\0";

const char* fragmentShaderSource = "#version 330 core\n"
"in vec3 outColor;\n"
"out vec4 fragcolor;\n"
"void main(){\n"
"	fragcolor = vec4(outColor,1.0f);\n"
"}\n\0";

const vector<float> vertexPositions = {

	//vertex                      //color               
	-0.5f,-.25f,0.0f,             1.0f,0.0f,0.0f,
	0.5f,-.75f,0.0f,              0.0f,1.0f,0.0f,
	0.0f,0.5f,0.0f,               0.0f,0.0f,1.0f,

	-0.5f,-0.5f,0.0f,             0.0f,1.0f,0.0f,
	0.5f,-0.5f,0.0f,              1.0f,0.0f,0.0f,
	0.0f,-1.0f,0.0f,              0.0f,0.0f,1.0f
};


void shaderFile(const GLchar* vertexPath, const GLchar* fragmentPath, string& vertcode, string& fragcode) {
	ifstream vshaderFile;
	ifstream fshaderFile;
	vshaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	fshaderFile.exceptions(ifstream::failbit | ifstream::badbit);
	try {
		vshaderFile.open(vertexPath);
		fshaderFile.open(fragmentPath);
		stringstream vshaderStream, fshaderStream;
		//read the file into data stream
		vshaderStream << vshaderFile.rdbuf();
		fshaderStream << fshaderFile.rdbuf();
		//close file
		vshaderFile.close();
		fshaderFile.close();
		//transform the stream int string
		vertcode = vshaderStream.str();
		fragcode = fshaderStream.str();
	}
	catch (ifstream::failure e) {
		cout << "Error:: shader:: file not succesfully read" << endl;
	}
};

GLuint CreateShader(GLenum eType, const string& strShaderFile) {
	int shaderHandle = glCreateShader(eType);    //create shader
	const char* strFileData = strShaderFile.c_str();
	glShaderSource(shaderHandle, 1, &strFileData, NULL);    //bind with GLSL shader
	glCompileShader(shaderHandle);      //compile

	GLint status;          //check the error
	glGetShaderiv(shaderHandle, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint infoLogLength;
		glGetShaderiv(shaderHandle, GL_INFO_LOG_LENGTH, &infoLogLength);
		GLchar* infoLog = new GLchar[infoLogLength + 1];
		glGetShaderInfoLog(shaderHandle, infoLogLength, NULL, infoLog);

		const char* strShaderType = NULL;
		switch (eType) {
		case GL_VERTEX_SHADER: strShaderType = "vertex"; break;
		case GL_GEOMETRY_SHADER: strShaderType = "geometry"; break;
		case GL_FRAGMENT_SHADER: strShaderType = "fragment"; break;
		}
		fprintf(stderr, "Compile failure in %s shader:\n%s\n", strShaderType, infoLog);
		delete[] infoLog;
	}
	return shaderHandle;
}

GLuint CreateProgram(const vector<GLuint>& shaderList) {
	GLuint programHandle = glCreateProgram();
	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
		glAttachShader(programHandle, shaderList[iLoop]);      //attach shaders and program
	}

	glLinkProgram(programHandle);      //link program

	GLint status;            //check if there is error
	glGetProgramiv(programHandle, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint InfoLogLength;
		glGetProgramiv(programHandle, GL_INFO_LOG_LENGTH, &InfoLogLength);

		GLchar* InfoLog = new GLchar[InfoLogLength + 1];
		glGetProgramInfoLog(programHandle, InfoLogLength, NULL, InfoLog);
		fprintf(stderr, "Linkder failure: %s\n", InfoLog);
		delete[] InfoLog;
	}

	for (size_t iLoop = 0; iLoop < shaderList.size(); iLoop++) {
		glDetachShader(programHandle, shaderList[iLoop]);
	}
	return programHandle;
}

int VertexBuffer(GLuint& VAO, GLuint& VBO, GLuint& EBO, int flag, const vector<GLfloat>& vertices, const vector<GLuint>& verindex) {
	int sz = vertices.size();
	int stride = flag == 1 ? 6 : 9;

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	if (flag == 3) {
		glGenBuffers(1, &EBO);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexcount * sizeof(unsigned int), &verindex[0], GL_STATIC_DRAW);
	}
	glBufferData(GL_ARRAY_BUFFER, sz * sizeof(float), &vertices[0], GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), 0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);
	if (flag != 1) {
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (const void*)(6 * sizeof(GLfloat)));
		glEnableVertexAttribArray(2);
	}

	if (flag == 1)
		sz = 12;
	else
		sz = sz / (float)3;

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return sz;
}

#endif // !BUFFER_PROM
