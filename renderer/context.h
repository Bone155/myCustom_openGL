#pragma once
#include "glew/GL/glew.h"
#include "glfw//glfw3.h"
#include "glm/glm.hpp"

class context
{
	// Forward declaration -- declaring a type and using it here
	struct GLFWwindow *window;
public:
	bool init(int width, int height, const char *tittle);
	void tick();
	void term();
	void clear();

	bool shouldClose() const;

	float time() const;
	void resetTime(float resetValue);

	glm::vec3 inputDirection();
	glm::vec3 rotateAxis();
};

