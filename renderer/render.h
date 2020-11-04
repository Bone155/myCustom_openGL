#pragma once

#include "glew/GL/glew.h"
#include "glm/glm.hpp"

// defining the vertex structure
struct vertex
{
	glm::vec4 pos;
	glm::vec4 color;
	glm::vec2 uv;	// texture coordinates
	glm::vec4 normal;
};

// an object to represent a mesh
struct geometry 
{
	GLuint vao, vbo, ibo; // buffers
	GLuint size;		  // index count
};

// an object to represent out shader
struct shader
{
	GLuint program;
};

struct texture
{
	GLuint handle;
	unsigned width, height, channels;
};

struct light
{
	glm::vec3 direction;	// should be normalized
	glm::vec3 color;
};

// functions to make and unmake the above types
geometry loadGeometry(const char *filePath);

geometry makeGeometry(vertex* vertices, size_t vertCount,
	unsigned int* indices, size_t indexCount);

void freeGeometry(geometry &geo);

texture loadTexture(const char* filePath);
texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels);
void freeTexture(texture &tex);

shader makeShader(const char* vertSource, const char* fragSource);

shader loadShader(const char* vertPath, const char* fragPath);

void freeShader(shader &shad);

void draw(const shader &shad, const geometry &geo);

void setUniform(const shader& shad, GLuint location, const glm::mat4 &value);
void setUniform(const shader& shad, GLuint location, const texture &tex, int textureSlot);
void setUniform(const shader& shad, GLuint location, float value);
void setUniform(const shader& shad, GLuint location, const glm::vec3 &value);
