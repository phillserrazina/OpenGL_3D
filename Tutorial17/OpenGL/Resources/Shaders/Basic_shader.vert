#version 330
layout (location=0) in vec4 vertexPos;
layout (location=1) in vec4 vertexColour;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec4 colour;

void main()
{
	colour = vertexColour;
	gl_Position = projection * view * model * vertexPos;
}