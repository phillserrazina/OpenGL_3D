#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal; 
out vec3 Vertex;

out vec4 colour;
out vec4 LightData;

uniform vec4 lightDirection;
uniform vec4 lightDiffuseColour;

out mat3 TBN;

void main()
{

	// make sure light direction vector is unit length (store in L)
	vec4 L = normalize(lightDirection);
	vec3 T = normalize(vec3(model * vec4(tangent, 0.0)));
	vec3 BT = normalize(vec3(model * vec4(bitangent, 0.0)));
	vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
	TBN = transpose(mat3(T, BT, N));
	
	/// calculate light intensity per-vertex (Gouraud shading)
	float lightIntensity = dot(L.xyz, N);
	float li = clamp(lightIntensity, 0.0, 1.0);

	LightData = lightDiffuseColour * li;
	
	TexCoord = texCoord;
	
	Normal = mat3(transpose(inverse(model))) * normal;  // normal vector in eye coordinates
	
	Vertex = vec3(model * vec4(vertexPos, 1.0)); // vertex in eye coordinates (map to vec3 since gl_Vertex is a vec4)

	gl_Position = projection * view * model * vec4(vertexPos, 1.0f);
}