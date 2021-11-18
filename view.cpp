#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <fstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "mesh.h"
#include "buffer_prom.h"
using namespace std;

Mesh mesh;

const int WID = 800; //window width
const int HGT = 600; //window height

//key_board control
glm::vec3 Front = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::vec3 keybdscale = glm::vec3(1.0f, 1.0f, 1.0f);
bool keys[1024];
int control = 0;
//mouse control
GLfloat pitch = 0.0f, yaw = -90.0f;
GLfloat aspect = 45.0f;
GLfloat lastX = WID / 2.0f;
GLfloat lastY = HGT / 2.0f;


inline void processInput(GLFWwindow* window);
inline void frame_buffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode);
void do_movement();
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);

int main() {
	int flag = 1;
	cout << "**************************************************************" << endl;
	cout << "Control?  0--No, it's time to have a rest.   1--Yes, kayboard." << endl
		 << "                  2--Yes, definitely! Mouse control please.  " << endl;
	cout << "**************************************************************" << endl;
	cin >> control;
	cout << "**************************************************************" << endl;
	cout << "Render directly or from an obj file?  1--Render anyway" << endl
		 <<"                   2--read a file  3--read a file + EBO" << endl;
	cout << "**************************************************************" << endl;
	cin >> flag;
	if (flag < 1 || flag > 3 || control < 0 || control > 2) {
		cerr << "Input overflows. Program terminates!" << endl;
		return -1;
	}
	if (flag != 1) {
		char map[] = "data\\cube.obj";
		mesh.readFile(map);
	}
	const vector<GLfloat> vertices = mesh.transform();
	const vector<GLfloat> verticesOnly = mesh.transformIndecise();
	const vector<GLuint> verindex = mesh.indextrans();

	indexcount = verindex.size();

	//Initialize()
	glfwInit();
	GLFWwindow* window = glfwCreateWindow(WID, HGT, "objects", NULL, NULL);
	if (window == nullptr) {
		cout << "failed to create GLFW window." << endl;
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	if (control == 1)
		glfwSetKeyCallback(window, key_callback);
	else if (control == 2) {
		glfwSetCursorPosCallback(window, mouse_callback);
		glfwSetScrollCallback(window, scroll_callback);
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	glfwSetFramebufferSizeCallback(window, frame_buffer_size_callback);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "failed to initialize glad" << std::endl;
		return -1;
	}
	glEnable(GL_DEPTH_TEST);

	//build and compile shaders
	const GLchar vertexPath[] = "source_phong.vs";
	const GLchar fragmentPath[] = "source_phong.fs";
	string vertexcode, fragmentcode;
	GLuint vertexShader, fragmentShader;
	shaderFile(vertexPath, fragmentPath, vertexcode, fragmentcode);
	if (flag != 1) {
		vertexShader = CreateShader(GL_VERTEX_SHADER, vertexcode);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentcode);

	}
	else {
		vertexShader = CreateShader(GL_VERTEX_SHADER, vertexShaderSourceAdd);
		fragmentShader = CreateShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
	}

	//program
	vector<GLuint> shaderList;
	shaderList.push_back(vertexShader);
	shaderList.push_back(fragmentShader);
	GLuint programHandle = CreateProgram(shaderList);
	std::for_each(shaderList.begin(), shaderList.end(), glDeleteShader);

	//Bind vertax buffer
	GLuint VAO, VBO, EBO;
	int siz=0;
	if (flag == 1) {
		siz = VertexBuffer(VAO, VBO, EBO, flag, vertexPositions, verindex);
	}
	else if (flag == 2) {
		siz = VertexBuffer(VAO, VBO, EBO, flag, vertices, verindex);
		cameraPos = glm::vec3(0.0f, 0.0f, max(max(abs(vertices[2]),abs(vertices[1])),abs(vertices[0])) * 6);
		cameraFront = glm::vec3(0.0f, 0.0f, max(max(abs(vertices[2]), abs(vertices[1])), abs(vertices[0])) * 3 - 1);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else {
		siz = VertexBuffer(VAO, VBO, EBO, flag, verticesOnly, verindex);
		cameraPos = glm::vec3(0.0f, 0.0f, max(max(abs(vertices[2]), abs(vertices[1])), abs(vertices[0])) * 6);
		cameraFront = glm::vec3(0.0f, 0.0f, max(max(abs(vertices[2]), abs(vertices[1])), abs(vertices[0])) * 3 - 1);
		cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
	}

	//while loop to render
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		do_movement();

		processInput(window);

		glClearColor(0.745f, 0.745f, 0.745f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glUseProgram(programHandle);

		glm::mat4 viewport;  //the perspective camera
		viewport = glm::lookAt(cameraPos, cameraFront, cameraUp);
		//viewport = glm::lookAt(glm::vec3(0.0f, 0.0f, abs(vertices[2]) * 6), glm::vec3(0.0f, 0.0f, abs(vertices[2] * 3 - 1)), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 model = glm::mat4(1.0f);   //rotation
		if (control != 2) {
			model = glm::rotate(model, 0.5f * (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, keybdscale);
		}
		else {
			model = glm::rotate(model, 0.2f * yaw, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::rotate(model, 0.2f * pitch, glm::vec3(1.0f, 0.0f, 0.0f));
		}
		//model = glm::translate(model, glm::vec3(1.0f, 0.8f, -0.4f));
		glm::mat4 projection = glm::mat4(1.0f);  //projection on the screen
		projection = glm::perspective(aspect, (float)WID / (float)HGT, 0.1f, 100.0f);

		GLuint modelLoc = glGetUniformLocation(programHandle, "model");
		GLuint viewpointLoc = glGetUniformLocation(programHandle, "viewport");
		GLuint projectionLoc = glGetUniformLocation(programHandle, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewpointLoc, 1, GL_FALSE, glm::value_ptr(viewport));
		glUniformMatrix4fv(projectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

		//changable color
		srand((unsigned int)time(NULL));//float timevalue = glfwGetTime();
		float valuearea_x = rand() % 256 / (float)256;//sin(timevalue) / 2.0f + .5f; //set color value between [0,1]
		float valuearea_y = rand() % 256 / (float)256;// cos(timevalue) / 2.0f + .5f; //set color value between [0,1]
		float valuearea_z = rand() % 256 / (float)256;
		GLuint vertexColorLocation = glGetUniformLocation(programHandle, "ourColor");
		glUniform4f(vertexColorLocation, valuearea_x, valuearea_y, valuearea_z, 1.0f);
		//light and camera position
		GLuint lightDir = glGetUniformLocation(programHandle, "lightPos");
		GLuint lightColor = glGetUniformLocation(programHandle, "lightColor");
		GLuint viewPos = glGetUniformLocation(programHandle, "viewPos");
		glUniform3f(lightDir, 2.0f, -2.0f, -2.0f);
		glUniform3f(lightColor, 1.0f, 1.0f, 1.0f);
		glUniform3f(viewPos, cameraPos.x, cameraPos.y, cameraPos.z);

		glBindVertexArray(VAO);
		if (flag == 3)
			glDrawElements(GL_TRIANGLES, indexcount, GL_UNSIGNED_INT, 0);
		else
			glDrawArrays(GL_TRIANGLES, 0, siz);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glUseProgram(0);

	glfwTerminate();
	return 0;
}

inline void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

inline void frame_buffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, GL_TRUE);
	}
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS) {
			keys[key] = true;
		}
		else if (action == GLFW_RELEASE) {
			keys[key] = false;
		}
	}
}

void do_movement() {
	GLfloat cameraSpeed = 0.05f;
	if (keys[GLFW_KEY_1]) {
		keybdscale += 0.01f * glm::vec3(1.0f, 1.0f, 1.0f);
	}
	if (keys[GLFW_KEY_2]) {
		keybdscale -= 0.01f * glm::vec3(1.0f, 1.0f, 1.0f);
		if (keybdscale[0] <= 0)
			keybdscale = glm::vec3(0.0f, 0.0f, 0.0f);
	}
	if (keys[GLFW_KEY_W]) {
		cameraPos += cameraSpeed * Front;
		cameraFront += cameraSpeed * Front;
	}
	if (keys[GLFW_KEY_S]) {
		cameraPos -= cameraSpeed * Front;
		cameraFront += cameraSpeed * Front;
	}
	if (keys[GLFW_KEY_UP]) {
		cameraPos += cameraSpeed * cameraUp;
		cameraFront += cameraSpeed * cameraUp;
	}
	if (keys[GLFW_KEY_DOWN]) {
		cameraPos -= cameraSpeed * cameraUp;
		cameraFront -= cameraSpeed * cameraUp;
	}
	if (keys[GLFW_KEY_LEFT]) {
		cameraPos -= glm::normalize(glm::cross(Front, cameraUp)) * cameraSpeed;
		cameraFront -= cameraSpeed * glm::normalize(glm::cross(Front, cameraUp));
	}
	if (keys[GLFW_KEY_RIGHT]) {
		cameraPos += glm::normalize(glm::cross(Front, cameraUp)) * cameraSpeed;
		cameraFront += cameraSpeed * glm::normalize(glm::cross(Front, cameraUp));
	}
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	GLfloat xoffset = xpos - lastX;
	GLfloat yoffset = lastY - ypos;
	lastX = xpos;
	lastY = ypos;

	//GLfloat sensitivity = 0.01f;
	GLfloat roteangle = 0.05f;
	xoffset *= roteangle;
	yoffset *= roteangle;

	yaw += xoffset;
	pitch += yoffset;

	/*if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;*/

	/*glm::vec3 mousefront;
	mousefront.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	mousefront.y = sin(glm::radians(pitch));
	mousefront.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront += glm::normalize(mousefront);*/
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
	if (aspect >= 1.0f && aspect <= 45.0f)
		aspect -= yoffset;
	if (aspect <= 1.0f)
		aspect = 1.0f;
	if (aspect >= 45.0f)
		aspect = 45.0f;
}