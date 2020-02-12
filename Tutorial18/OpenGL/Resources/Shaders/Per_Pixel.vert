#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 VertexNormal;

void main()
{
	TexCoord = texCoord;
	VertexNormal = vertexNormal;
	gl_Position = projection * view * model * vec4(vertexPos, 1.0f);
}