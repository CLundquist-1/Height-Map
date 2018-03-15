#include<iostream>
#include "Shader.h"
#include<GL/glew.h>
#include<GLFW\glfw3.h>
#include<GL\freeglut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>

#include "Image.h"
#include "HeightMap.h"

using std::cout;
using std::getchar;
using std::vector;
using std::endl;

//Camera Options
int pressCam = 0;
bool point = false;
int pressPoint = 0;
int pressWire = 0;
bool wire = false;
bool freeCam = false;

constexpr unsigned int scr_width = 800;
constexpr unsigned int scr_height = 600;

float lastX = scr_width * 0.5, lastY = scr_height * 0.5;
bool firstMouse = true;
float yaw = -90.0f, pitch = 0.0f, fov = 45.0f;
float lastFrame = 0.0f; // Time of last frame
float deltaTime = 0.1f;	// Time between current frame and last frame

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 25.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);
glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
glm::mat4 proj = glm::perspective(glm::radians(45.0f), (float)scr_width / (float)scr_height, 0.1f, 100.0f);
glm::mat4 model = glm::mat4(1.0f);

const char* file = "two.png";
const char *vertexShaderSource = "vertexShader.vs";
const char *fragmentShaderSource = "fragmentShader.fs";

constexpr float heightStep = 10;
constexpr float widthStep = 5;
constexpr float lengthStep = 5;
unsigned int heightMapElements;

//TODO: Establish VAOs (which store enabled vertex attributes, attribute configurations, vertex buffer objects, etc.)
unsigned int vao;
//TODO End: Just like that

/////////////////////////////////////////Lets put all the drawing initialization in one place////////////////////////////////////
Shader initDrawing() {
	Image image = Image(file);
	image.PrintPixels();

	HeightMap hm = HeightMap(image, heightStep, widthStep, lengthStep);

	heightMapElements = hm.Elements().size();

	hm.PrintVertices();

	hm.PrintElements();


	//TODO: Generate veretx arrays and load them with data
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	unsigned int vbo;
	glGenBuffers(1, &vbo);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	unsigned int ebo;
	glGenBuffers(1, &ebo);

	//Fill Array Buffer
	glBufferData(GL_ARRAY_BUFFER, hm.Vertices().size() * sizeof(float), &hm.Vertices()[0], GL_STATIC_DRAW);

	//Explain data layout of the array to the gpu
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//What layout location we are setting, size of the attribute, type of data, if we want numbers not from 0 to 1 to be normalized (mapped ot them), stride, offset to beginning
	glEnableVertexAttribArray(0);	//This turns layout location 0

	//Bind and fill element array buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, hm.Elements().size() * sizeof(unsigned int), &hm.Elements()[0], GL_STATIC_DRAW);

	//TODO END: Just like that
	Shader shader(vertexShaderSource, fragmentShaderSource);
	shader.use();

	return shader;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	proj = glm::perspective(glm::radians(45.0f), (float)width / (float)height, 0.1f, 100.0f);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}
	//Calculate change in mouse position
	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; // reversed since y-coordinates range from bottom to top
	lastX = xpos;
	lastY = ypos;

	//Adjust for camera speed
	float sensitivity = 0.05f;
	xoffset *= sensitivity;
	yoffset *= sensitivity;

	//Constrain the pitch so there is no reversal of direction
	yaw += xoffset;
	pitch += yoffset;
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	//Change the front facing portion of the camera based on the pitch and yaw
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	cameraFront = glm::normalize(front);
}

////////////////////////////////////Check for polled input and process it/////////////////////////////////////////////////////
void processInput(GLFWwindow *window)
{
	pressCam = pressCam << 3;
	pressWire = pressWire << 3;
	pressPoint = pressPoint << 3;
	//Close Window
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	//Activate Wire Mode
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
		if (!(pressWire))
			wire = !wire;
		pressWire = pressWire | 1UL;
		if (wire)
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);			//Enter default mode
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);			//Enter wireframe mode
	}
	//Swtich between free roam and mounted camera
	if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
		if (!(pressCam))
			freeCam = !freeCam;
		pressCam = pressCam | 1UL;
		if(freeCam)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	//Switch between Points and Triangle Drawings
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		if (!(pressPoint))
			point = !point;
		pressPoint = pressPoint | 1UL;
	}
	//Free Camera Movement
	if (freeCam) {
		float cameraSpeed = 40.0f * deltaTime; // adjust accordingly
		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
			cameraPos += cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
			cameraPos -= cameraSpeed * cameraFront;
		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
			cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
			cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
			cameraPos += glm::vec3(0.0f, 1.0f, 0.0f) * cameraSpeed;
		if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
			cameraPos += glm::vec3(0.0f, -1.0f, 0.0f) * cameraSpeed;
	}
}

int main() {
	if (!glfwInit()) {
		cout << "Failed to initialize GLFW\n";
		return -1;
	}
	//////////////////////////////Creation of our context window////////////////////////////////////////////////
	glfwWindowHint(GLFW_SAMPLES, 4); // 4x antialiasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); // We want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // To make MacOS happy; should not be needed
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // We don't want the old OpenGL 

																   // Open a window and create its OpenGL context
	GLFWwindow* window; // (In the accompanying source code, this variable is global for simplicity)
	window = glfwCreateWindow(scr_width, scr_height, "Tutorial 01", NULL, NULL);
	if (window == NULL) {
		fprintf(stderr, "Failed to open GLFW window. If you have an Intel GPU, they are not 3.3 compatible. Try the 2.1 version of the tutorials.\n");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//When focus is given to the window
	//glfwSetWindowFocusCallback(window, window_focus_callback);
	//Hide and capture mouse input
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	//Direct mouse input
	glfwSetCursorPosCallback(window, mouse_callback);

	//Scrolling
	//glfwSetScrollCallback(window, scroll_callback);

	// Initialize GLEW
	glewExperimental = true; // Needed in core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		return -1;
	}

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	Shader shader = initDrawing();

	//We do this outside the main loop because the projection matrix does not change
	unsigned int projLoc = glGetUniformLocation(shader.ID, "project");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(proj));

	//Lets take depth into account for drawing
	glEnable(GL_DEPTH_TEST);

	glPointSize(10);

	//TODO: GetLocaiton info for later use
	unsigned int modelLoc = glGetUniformLocation(shader.ID, "model");
	unsigned int viewLoc = glGetUniformLocation(shader.ID, "view");
	unsigned int colorLoc = glGetUniformLocation(shader.ID, "ourColor");
	//TODO END: Just like that

	lastFrame = glfwGetTime();
	//glEnable(GL_LINE_SMOOTH);
	/////////////////////////////Render Loop///////////////////////////////////////
	while (!glfwWindowShouldClose(window))
	{
		glPointSize(1);
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		//deltaTime = 0.01f;

		processInput(window);

		//Clear Buffer
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);			//There is also a depth buffer bit and stencil buffer bit

		if (freeCam)
			view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

		//TODO: Set uniforms before draw

		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
		glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);				//This is a different way of passing the matrix, giving it the address of the first element
		glUniform3f(colorLoc, 1.0f, 1.0f, 1.0f);

		//TDOD END: Just like that

		//TODO: Bind and draw

		glBindVertexArray(vao);
		//glDrawArrays(GL_TRIANGLES, 0, 3);		//Primitive, Starting Index of Vertex Array, Number of Vertices
		glDrawElements(GL_TRIANGLES, heightMapElements, GL_UNSIGNED_INT, 0);		//Primitve, Number of Elements, tpye of indices, offset or index array

		//TODO End: Just like that



		//Swap buffers
		glfwSwapBuffers(window);

		//Poll input events
		glfwPollEvents();
	}

	glfwTerminate();
	return 0;
}