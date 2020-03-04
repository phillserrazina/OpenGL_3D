#version 330 core

layout (location = 0) in vec3 vertexPos;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoord;
layout (location = 3) in ivec4 BoneIDs;
layout (location = 4) in vec4 Weights;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 TexCoord;
out vec3 Normal; 
out vec3 Vertex; 

const int MAX_BONES = 150;
uniform mat4 gBones[MAX_BONES];

void main()
{
    mat4 BoneTransform = gBones[BoneIDs[0]] * Weights[0];
    BoneTransform += gBones[BoneIDs[1]] * Weights[1];
    BoneTransform += gBones[BoneIDs[2]] * Weights[2];
    BoneTransform += gBones[BoneIDs[3]] * Weights[3];

	vec4 PosL = BoneTransform * vec4(vertexPos, 1.0);

	TexCoord = texCoord;
	Normal = vec3(BoneTransform * vec4(normal, 1.0));  // normal vector in eye coordinates
	Vertex = vec3(model * vec4(vertexPos, 1.0)); // vertex in eye coordinates (map to vec3 since gl_Vertex is a vec4)

	gl_Position = projection * view * model * PosL;
}