#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 vertexNormal;
layout (location = 2) in vec2 texCoord;

// texture sampler
uniform sampler2D texture_diffuse1;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;


// basic directional light model
uniform vec4		lightDirection; // direction light comes FROM
uniform vec4		lightDiffuseColour;


out vec2 TexCoord;
out vec4 LightData;

void main()
{
	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(lightDirection);

	// important to normalise length of normal otherwise shading artefacts occur
	vec3 N = normalize(vertexNormal);
	
	// calculate light intensity per-vertex (Gouraud shading)
	float lightIntensity = dot(L.xyz, N);
	float li = clamp(lightIntensity, 0.0, 1.0);

	LightData = lightDiffuseColour * li;

	TexCoord = texCoord;
	gl_Position = projection * view * model * vec4(vertexPos, 1.0f);
}