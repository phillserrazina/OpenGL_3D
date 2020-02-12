#version 330 core

in vec2 TexCoord;
in vec4 LightData;

// texture sampler
uniform sampler2D texture_diffuse1;

out vec4 FragColor;

void main()
{
	//vec4 finalColour = texture(texture_diffuse1, TexCoord) * LightData;
	vec4 finalColour = vec4(1.0, 1.0, 1.0, 1.0) * LightData; //Ignore texture colour
	FragColor = finalColour;
}