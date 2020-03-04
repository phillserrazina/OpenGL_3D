#version 330 core

in vec2 TexCoord;
in vec3 Normal; 
in vec3 Vertex; 

//Texture sampler
uniform sampler2D texture_diffuse2;

out vec4 FragColour;

void main()
{
	vec4 texColour = texture(texture_diffuse2, TexCoord);

	FragColour = texColour;
}