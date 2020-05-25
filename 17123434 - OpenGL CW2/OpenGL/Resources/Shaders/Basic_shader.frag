#version 330 core

in vec2 TexCoord;
in vec3 Normal; 
in vec3 Vertex; 
in mat3 TBN;

//Texture sampler
uniform sampler2D texture_diffuse1;

//Camera location
uniform vec3 eyePos;

//Light information
uniform vec3		lightAttenuation; // x=constant, y=linear, z=quadratic (x<0 means light is not active)

//Material information
uniform vec4		matAmbient;
uniform vec4		matDiffuse;
uniform vec4        matSpecularColour;
uniform float       matSpecularExponent;

// Point Light Info
const int			NUM_OF_LIGHTS = 4;
uniform vec4		lightAmbArray[NUM_OF_LIGHTS];
uniform vec4		lightPosArray[NUM_OF_LIGHTS];
uniform vec4		lightColArray[NUM_OF_LIGHTS];

// Directional Light Info
in vec4 LightData;

out vec4 FragColour;

vec4 calculateLight(vec4 lightAmbient, vec4 lightPosition, vec4 lightColour, bool isDirectional);

void main()
{
	// Final colour is the combination of all components
	for (int i = 0; i < NUM_OF_LIGHTS; i++) {
		vec4 l = calculateLight(lightAmbArray[i], lightPosArray[i], lightColArray[i], i == 0);
		FragColour += l;
	}
}

vec4 calculateLight(vec4 lightAmbient, vec4 lightPosition, vec4 lightColour, bool isDirectional) {
	if (isDirectional) {
		return texture(texture_diffuse1, TexCoord) * LightData;
	}

	//Attenuation/drop-off	
	float d = length(lightPosition.xyz - Vertex);
	float att = 1.0 / (lightAttenuation.x + lightAttenuation.y * d + lightAttenuation.z * (d * d));

	//Ambient light value
	vec4 texColour = texture(texture_diffuse1, TexCoord);
	vec4 ambient = lightAmbient * matAmbient * texColour * att;

	//Diffuse light value
	vec3 N = normalize(TBN * Normal);
	
	vec3 L;
	L = normalize(lightPosition.xyz - Vertex);

	float lambertTerm = clamp(dot(N, L), 0.0, 1.0);
	vec4 diffuse = lightColour * matDiffuse * lambertTerm * texColour * att;

	//Specular light value
	vec3 E = normalize(eyePos - Vertex);
	vec3 R = reflect(-L, N); // reflected light vector about normal N
	float specularIntensity = pow(max(dot(E, R), 0.0), matSpecularExponent);
	vec4 specular = matSpecularColour * specularIntensity * texColour * att;

	//Final colour is the combinatin of all components
	return ambient + diffuse + specular;
}