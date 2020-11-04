#version 430

in vec4 vColor;
in vec2 vUV;

out vec4 outputColor;

layout (location = 3) uniform sampler2D mainTexture;
layout (location = 4) uniform float time;

void main() 
{ 
	outputColor = texture(mainTexture, vUV);
	outputColor.r *= sin(time);
};