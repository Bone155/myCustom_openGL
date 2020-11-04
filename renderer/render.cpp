#include "render.h"

#include <vector>
#include <string>
#include <fstream>

#include "glm/ext.hpp"
#include "objLoader/tiny_obj_loader.h"
#include "stb/stb_image.h"

geometry loadGeometry(const char* filePath)
{
	// load up all data from file
	tinyobj::attrib_t vertexAttributes;

	// double check that everything's ok
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	std::string error;

	bool success = tinyobj::LoadObj(&vertexAttributes, &shapes, &materials, &error, filePath);

	if (!success) {
		fprintf(stderr, error.c_str());
		return {};
	}

	//get mesh data
	std::vector<vertex> vertices;
	std::vector<unsigned int> indices;

	// form geometry data from mesh data by tinyObj
	size_t offset = 0;
	for (size_t i = 0; i < shapes[0].mesh.num_face_vertices.size(); i++) {
		unsigned char faceVertices = shapes[0].mesh.num_face_vertices[i];

		assert(faceVertices == 3 && "unsupported number of face verticies");

		// iterating over vertex data to fit our own vertex format
		for (unsigned char j = 0; j < faceVertices; j++) {
			tinyobj::index_t idx = shapes[0].mesh.indices[offset + j];

			tinyobj::real_t posX = vertexAttributes.vertices[3 * idx.vertex_index + 0];
			tinyobj::real_t posY = vertexAttributes.vertices[3 * idx.vertex_index + 1];
			tinyobj::real_t posZ = vertexAttributes.vertices[3 * idx.vertex_index + 2];

			tinyobj::real_t colR = 1.0f;
			tinyobj::real_t colG = 1.0f;
			tinyobj::real_t colB = 1.0f;

			tinyobj::real_t texU = vertexAttributes.texcoords[2 * idx.texcoord_index + 0];
			tinyobj::real_t texV = vertexAttributes.texcoords[2 * idx.texcoord_index + 1];

			tinyobj::real_t norX = vertexAttributes.normals[3 * idx.normal_index + 0];
			tinyobj::real_t norY = vertexAttributes.normals[3 * idx.normal_index + 1];
			tinyobj::real_t norZ = vertexAttributes.normals[3 * idx.normal_index + 2];

			vertices.push_back(vertex{
					{ posX, posY, posZ, 1.0f },
					{ colR, colG, colB, 1.0f },
					{ texU, texV },
					{ norX, norY, norZ, 0.0f }
				});
			indices.push_back(faceVertices * i + j);
		}
		offset += faceVertices;
	}

	// return makeGeometry using the data from tinyObj
	return makeGeometry(vertices.data(), vertices.size(), indices.data(), indices.size());
}

geometry makeGeometry(vertex* verts, size_t vertCount, unsigned int* indices, size_t indexCount)
{
	// make an instance geometry
	geometry newGeo = {};
	newGeo.size = indexCount;

	// generate buffers and VAO
	glGenBuffers(1, &newGeo.vbo);
	glGenBuffers(1, &newGeo.ibo);
	glGenVertexArrays(1, &newGeo.vao);

	// bind VAO and buffers
	glBindVertexArray(newGeo.vao);
	glBindBuffer(GL_ARRAY_BUFFER, newGeo.vbo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, newGeo.ibo);

	// populate the buffers (send data over)
	glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(vertex), verts, GL_STATIC_DRAW);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexCount * sizeof(unsigned int), indices, GL_STATIC_DRAW);

	//auto off = offsetof(vertex, pos);

	// describe the data contained within the buffers
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,			  // position
						  4,			  // how many things?
						  GL_FLOAT,		  // what types of thingsare in that thing
						  GL_FALSE,		  // normalize this?
						  sizeof(vertex), // byte offset between vertices
						  (void*)0);	  // byte offset within a vertex to get this data

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,			  // vertex color
						  4,			  // how many things?
						  GL_FLOAT,		  // what types of thingsare in that thing
						  GL_FALSE,		  // normalize this?
						  sizeof(vertex), // byte offset between vertices
						  (void*)offsetof(vertex, color));	  // byte offset within a vertex to get this data - 8

	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2,			  // uv
						  2,			  // how many things?
						  GL_FLOAT,		  // what types of things are in that thing
						  GL_FALSE,		  // normalize this?
						  sizeof(vertex), // byte offset between vertices
						  (void*)offsetof(vertex, uv));	  // byte offset within a vertex to get this data - 32

	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3,			  // uv
						  4,			  // how many things?
						  GL_FLOAT,		  // what types of things are in that thing
						  GL_FALSE,		  // normalize this?
						  sizeof(vertex), // byte offset between vertices
						  (void*)offsetof(vertex, normal));	  // byte offset within a vertex to get this data - 40

	// unbind buffers (VAO then the buffers)
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	// return the geo
	return newGeo;
}

void freeGeometry(geometry &geo)
{
	glDeleteBuffers(1, &geo.vbo);
	glDeleteBuffers(1, &geo.ibo);
	glDeleteVertexArrays(1, &geo.vao);

	geo = {};
}

texture loadTexture(const char* filePath)
{
	// create some variables to hold onto some data
	int imageWidth, imageHeight, imageFormat;
	unsigned char* rawPixelData = nullptr;

	// use stb to load up image (and set some settings, where neccessary)
	stbi_set_flip_vertically_on_load(true);
	rawPixelData = stbi_load(filePath, &imageWidth, &imageHeight, &imageFormat, STBI_default);
	assert(rawPixelData != nullptr && "Image failed to load");

	// pass image over to makeTexture
	texture tex = makeTexture(imageWidth, imageHeight, imageFormat, rawPixelData);
	assert(tex.handle != 0 && "Failed to create texture");

	// free image data
	stbi_image_free(rawPixelData);

	// return the texture
	return tex;
}

texture makeTexture(unsigned width, unsigned height, unsigned channels, const unsigned char* pixels)
{
	// figure out DGL texture format to use
	assert(channels > 0 && channels < 5); // only support 1-4 channels // safe programming
	GLenum oglFormat = GL_RED;

	switch (channels)
	{
		case 1:
			oglFormat = GL_RED;		// RED
			break;
		case 2:
			oglFormat = GL_RG;		// RED GREEN
			break;
		case 3:
			oglFormat = GL_RGB;		// RED GREEN BLUE
			break;
		case 4:
			oglFormat = GL_RGBA;	// RED GREEN BLUE ALPHA
			break;
	}

	// generate a texture
	texture tex = { 0, width, height, channels };
	glGenTextures(1, &tex.handle);

	// bind and buffer data to it
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	glTexImage2D(GL_TEXTURE_2D,			// texture to buffer data to
				 0, 					// level
				 oglFormat, 			// format specifier
				 width, 				// width pixels
				 height, 				// height pixels
				 0, 					// border value
				 oglFormat, 			// final format specifier
				 GL_UNSIGNED_BYTE, 		// type of data elements
				 pixels);				// pointer to actual data

	// set some filtering settings
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);	// filtering applied when texel density is greater than display (interpoluting existing data)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);	// filtering applied when texel density is less than display (extrapolating data)

	// return texture
	glBindTexture(GL_TEXTURE_2D, 0);

	return tex;
}

void freeTexture(texture &tex)
{
	glDeleteTextures(1, &tex.handle);
	tex = {};
}

bool checkShader(GLuint target, const char* humanReadableName = "")
{
	assert(glIsShader(target) && "target is not a shader -- cannot check");

	GLint shaderCompileStatus = 0;
	glGetShaderiv(target, GL_COMPILE_STATUS, &shaderCompileStatus);
	if (shaderCompileStatus != GL_TRUE)
	{
		GLsizei logLength = 0;
		GLchar message[1024];
		glGetShaderInfoLog(target, 1024, &logLength, message);
		fprintf(stderr, "\n[ERROR] %s Shader \n %s", humanReadableName, message);

		// return an empty shader if it fails
		return false;
	}

	return true;
}

shader loadShader(const char* vertPath, const char* fragPath)
{
	std::fstream vertFile(vertPath,			// path to the file
						  std::ios::in);	// the mode of operation

	assert(vertFile.is_open() && "Failed to open vertex shader file");
	std::string vertSource;
	for (std::string line; std::getline(vertFile, line);)
	{
		vertSource += line + "\n";
	}

	vertFile.close();

	std::fstream fragFile(fragPath,			// path to the file
						  std::ios::in);	// the mode of operation

	assert(fragFile.is_open() && "Failed to open frag shader file");
	std::string fragSource;
	for (std::string line; std::getline(fragFile, line);)
	{
		fragSource += line + "\n";
	}

	fragFile.close();

	return makeShader(vertSource.c_str(), fragSource.c_str());
}

shader makeShader(const char* vertSource, const char* fragSource)
{
	// make a shader program
	shader newShad = {};
	newShad.program = glCreateProgram();

	// create the shaders
	GLuint vert = glCreateShader(GL_VERTEX_SHADER);		// this is a vertex shader
	GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);	// this is a fragment shader

	// compile shaders
	glShaderSource(vert, 1, &vertSource, 0);  // send source code for specific shader
	glShaderSource(frag, 1, &fragSource, 0);
	glCompileShader(vert);					  // actually compile the shader
	glCompileShader(frag);

	// validate the shaders
	bool shaderOK = checkShader(vert, "Vertex");
	assert(shaderOK && "Vertex shader failed to compile");
	shaderOK = checkShader(frag, "Fragment");
	assert(shaderOK && "Fragment shader failed to complie");

	// attach shaders
	glAttachShader(newShad.program, vert);	// associate the vertex shader with the shader program
	glAttachShader(newShad.program, frag);	// associate the fragment shader with the shader program

	// link the shader program
	glLinkProgram(newShad.program);

	// delete the shaders
	glDeleteShader(vert);	// delete the vertex shader
	glDeleteShader(frag);	// delete the fragment shader

	// return the shader
	return newShad;
}

void freeShader(shader &shad)
{
	glDeleteShader(shad.program);
	shad = {};
}

void draw(const shader& shad, const geometry& geo)
{
	// specify which shader
	glUseProgram(shad.program);
	// specify which VAO
	glBindVertexArray(geo.vao);

	// draw
	glDrawElements(GL_TRIANGLES,	// primitive type
				   geo.size,		// indices
				   GL_UNSIGNED_INT,	// index type
				   0);
}

void setUniform(const shader& shad, GLuint location, const glm::mat4 &value)
{
	glProgramUniformMatrix4fv(shad.program, location, 1, GL_FALSE, glm::value_ptr(value));
}

void setUniform(const shader& shad, GLuint location, const texture& tex, int textureSlot)
{
	// setup this texture in a slot
	glActiveTexture(GL_TEXTURE0 + textureSlot);
	glBindTexture(GL_TEXTURE_2D, tex.handle);

	// pass the slot number over to the uniform
	glProgramUniform1i(shad.program, location, textureSlot);
}

void setUniform(const shader& shad, GLuint location, float value)
{
	glProgramUniform1f(shad.program, location, value);
}

void setUniform(const shader& shad, GLuint location, const glm::vec3& value)
{
	glProgramUniform3fv(shad.program, location, 1, glm::value_ptr(value));
}
