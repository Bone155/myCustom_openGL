#include "context.h" // header that is being implemented

// system libraries
#include <iostream> // cout
#include <cstdio> // printf
#include <cassert>

void APIENTRY errorCallBack(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam)
{
	//std::cerr << message << std::endl;

	fprintf(stderr, message);
}

bool context::init(int width, int height, const char* tittle)
{
	// initialize glfw
	int gflStatus = glfwInit();
	if (gflStatus == GLFW_FALSE) {
		printf("An error occured with GLFW");
		return false;
	}

	assert(gflStatus != GLFW_FALSE && "An error occured with GLFW");
	
	window = glfwCreateWindow(width, height, tittle, nullptr, nullptr);

	glfwMakeContextCurrent(window);

	// initailize glew
	int glewStatus = glewInit();
	if (glewStatus != GLEW_OK) {
		printf("An error occured with GLEW");
		return false;
	}

	// print out some diagnostics
	// GL_VERSION
	// GL_RENDERER
	// GL_VENDOR
	// GL_SHADING_LANGUAGE_VERSION
	printf("OpenGL Version: %s\n", (const char*)glGetString(GL_VERSION));
	printf("Renderer: %s\n", (const char*)glGetString(GL_RENDERER));
	printf("Vendor: %s\n", (const char*)glGetString(GL_VENDOR));
	printf("GLSL: %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	glEnable(GL_DEBUG_OUTPUT);
	glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

	glDebugMessageCallback(errorCallBack, nullptr);
	glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DEBUG_SEVERITY_LOW, 0, 0, true);

	// set up some good defaults
	glEnable(GL_CULL_FACE);   // ogl defaults to off
	glCullFace(GL_BACK);      // cull back faces
	glFrontFace(GL_CCW);      // ccw faces are front faces (ogl defaults to CCW)

	glClearColor(0.25f, 0.25f, 0.25f, 1.0f);

	// return true if everything's ok
	return true;
}

void context::tick()
{
	// update glfw
	glfwPollEvents();		 // update events, input, etc.
	glfwSwapBuffers(window); // update passing window

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void context::term()
{
	// clean up our window
	glfwDestroyWindow(window);
	window = nullptr;

	// clean up GLFW
	glfwTerminate();
}

void context::clear()
{
	// clears screen and replaces it with the color
	// given by the glCearColor function call
	glClear(GL_COLOR_BUFFER_BIT);
}

bool context::shouldClose() const
{
	return glfwWindowShouldClose(window);
}

float context::time() const
{
	return (float)glfwGetTime();
}

void context::resetTime(float resetValue)
{
	glfwSetTime(resetValue);
}

glm::vec3 context::inputDirection()
{
	glm::vec3 dir = glm::vec3();
	if (glfwGetKey(window, GLFW_KEY_W)) {
		dir = glm::vec3(0.0f, 0.001f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_S)) {
		dir = glm::vec3(0.0f, -0.001f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_A)) {
		dir = glm::vec3(-0.001f, 0.0f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_D)) {
		dir = glm::vec3(0.001f, 0.0f, 0.0f);
	}

	return dir;
}

glm::vec3 context::rotateAxis()
{
	glm::vec3 rotate = glm::vec3();
	if (glfwGetKey(window, GLFW_KEY_UP)) {
		rotate = glm::vec3(1.0f, 0.0f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_DOWN)) {
		rotate = glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_LEFT)) {
		rotate = glm::vec3(0.0f, -1.0f, 0.0f);
	}
	else if (glfwGetKey(window, GLFW_KEY_RIGHT)) {
		rotate = glm::vec3(0.0f, 1.0f, 0.0f);
	}
	else {
		rotate = glm::vec3(0.0f, 0.001f, 0.0f);
	}
	return rotate;
}
