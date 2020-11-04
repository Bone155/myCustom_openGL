#include "context.h"
#include "render.h"

#include "glm/ext.hpp"

int main() {
	context game;

	game.init(800, 512, "My OpenGL");

	glm::vec3 cameraPos = glm::vec3(5.0f, 2.0f, 7.0f);
	glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
	glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

	// direction the model will translate
	glm::vec3 moveDirection = glm::vec3();
	glm::vec3 rotateDirection = glm::vec3();

	// vertices
	vertex triVerts[] =
	{
		{ { -0.5f, -0.5f, 0, 1 }, { 0.0f, 0.0f, 1.0f, 1}, { 0, 0 }, { 0.0f, 0.0f, -1.0f, 0.0f } }, // bottom left
		{ {  0.5f, -0.5f, 0, 1 }, { 0.0f, 1.0f, 0.0f, 1}, { 1, 0 }, { 0.0f, 0.0f, -1.0f, 0.0f } },  // bottom right
		{ {  0.f,   0.5f, 0, 1 }, { 1.0f, 0.0f, 0.0f, 1}, { 0.5f, 1 }, { 0.0f, 0.0f, -1.0f, 0.0f } }  // top center
	};

	vertex quadVerts[] =
	{
		{ { -1.0f, -1.0f, 0, 1 }, { 0.0f, 0.0f, 0.0f, 1}, { 0,0 }, { 0.0f, 0.0f, -1.0f, 0.0f } },	// bottom left
		{ {  1.0f, -1.0f, 0, 1 }, { 1.0f, 0.0f, 0.0f, 1}, { 1,0 }, { 0.0f, 0.0f, -1.0f, 0.0f } },	// bottom right
		{ { -1.0f,  1.0f, 0, 1 }, { 0.0f, 0.0f, 1.0f, 1}, { 0,1 }, { 0.0f, 0.0f, -1.0f, 0.0f } },	// top left
		{ {  1.0f,  1.0f, 0, 1 }, { 0.0f, 1.0f, 0.0f, 1}, { 1,1 }, { 0.0f, 0.0f, -1.0f, 0.0f } }	// top right
	};

	// indices
	unsigned int triIndices[] = { 2, 0, 1 };
	unsigned int quadIndices[] = { 3, 2, 0, 0, 1, 3 };

	// make the geometry
	geometry triangle = makeGeometry(triVerts, 3, triIndices, 3);
	geometry quad = makeGeometry(quadVerts, 4, quadIndices, 6);
	geometry cube = loadGeometry("res\\cube.obj");
	geometry triObj = loadGeometry("res\\tri.obj");

	// load up textures
	texture terry = loadTexture("res\\terry.png");

	// make the shader
	shader basicShader = loadShader("res\\basic.vert", "res\\basic.frag");
	shader mvpShader = loadShader("res\\mvp.vert", "res\\basic.frag");
	shader lightShader = loadShader("res\\light.vert", "res\\light.frag");

	light sun = { {-1, 0, 0}, {1,1,1} };

	// setup matricies
	glm::mat4 proj = glm::perspective(glm::radians(80.0f), 800.f / 512.f, 0.1f, 100.f);
	glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);

	glm::mat4 triModel = glm::identity<glm::mat4>();

	shader choosenShader = lightShader;

	setUniform(choosenShader, 0, proj);
	setUniform(choosenShader, 1, view);
	setUniform(choosenShader, 2, triModel);

	setUniform(choosenShader, 3, terry, 0);

	setUniform(choosenShader, 5, { 0.1f, 0.1f, 0.1f });
	setUniform(choosenShader, 6, sun.color);
	setUniform(choosenShader, 7, sun.direction);

	triModel = glm::scale(triModel, glm::vec3(1.0f, 1.5f, 1.25f));

	while (!game.shouldClose())
	{
		game.tick();
		// implement game logic

		moveDirection = game.inputDirection();
		rotateDirection = game.rotateAxis();

		triModel = glm::translate(triModel, moveDirection);
		triModel = glm::rotate(triModel, glm::radians(0.05f), rotateDirection);
		
		game.clear();
		// implement render logics
		setUniform(choosenShader, 2, triModel);
		setUniform(choosenShader, 4, game.time());

		draw(choosenShader, cube);
	}

	freeGeometry(triangle);
	freeGeometry(quad);
	freeGeometry(cube);
	freeGeometry(triObj);

	freeShader(basicShader);
	freeShader(mvpShader);
	freeShader(lightShader);
	freeShader(choosenShader);

	freeTexture(terry);

	game.term();

	return 0;
}
