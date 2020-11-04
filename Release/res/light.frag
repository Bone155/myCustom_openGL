#version 430

in vec2 vUV;
in vec3 vNormal;

out vec4 outputColor;

layout (location = 3) uniform sampler2D mainTexture;

layout (location = 5) uniform vec3 ambient;
layout (location = 6) uniform vec3 lightDiffuse;
layout (location = 7) uniform vec3 lightDirection;

void main() 
{
	vec3 ambientColor = ambient;

	float lambert = max( 0.0f, dot(vNormal, -lightDirection) );

	vec3 diffuseColor = texture(mainTexture, vUV).xyz * lightDiffuse * lambert;

	outputColor = vec4(ambientColor + diffuseColor, 1.0f);
};