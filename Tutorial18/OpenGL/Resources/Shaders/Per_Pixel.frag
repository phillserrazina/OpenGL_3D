#version 330 core

in vec2 TexCoord;
in vec3 VertexNormal;

// Texture Sampler
uniform sampler2D texture_diffuse1;

// Basic directional light model
uniform vec4 lightDirection;
uniform vec4 lightDiffuseColour;

out vec4 FragColor;

void main()
{
	// Make sure light direction vector is unit length (store in L)
	vec4 L = normalize(lightDirection);

	// Important to normalize length of normal otherwise shading artefacts occur
	vec3 N = normalize(VertexNormal);

	// Calculate light intensity per-pixel (Phong shading)
	float lightIntensity = dot(L.xyz, N);
	float li = clamp(lightIntensity, 0.0, 1.0);

	vec4 LightData = lightDiffuseColour * li;
	
	vec4 finalColour = LightData; // Ignore texture colour

	FragColor = finalColour;
}