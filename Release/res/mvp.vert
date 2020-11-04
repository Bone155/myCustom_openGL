#version 430

layout (location = 0) in vec4 position;
layout (location = 1) in vec4 color;
layout (location = 2) in vec2 uv;

layout (location = 0) uniform mat4 proj;
layout (location = 1) uniform mat4 view;
layout (location = 2) uniform mat4 model;

layout (location = 4) uniform float time;	// time since application

out vec4 vColor;
out vec2 vUV;

void main()
{
	vec4 finalPosition = position;
	//finalPosition.x += sin(position.y + time) * 1.5f;
	//finalPosition.y += sin(position.z + time) * 2.5f;
	//finalPosition.z += sin(position.x + time) * 0.5f;

	gl_Position = proj * view * model * finalPosition;
	vColor = color;
	vUV = uv;
};