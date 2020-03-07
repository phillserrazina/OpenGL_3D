#include "Includes.h"

// Function prototypes
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// Camera settings
//							  width, heigh, near plane, far plane
Camera_settings camera_settings{ 1200, 1000, 0.1, 100.0 };

//Timer
Timer timer;

// Instantiate the camera object with basic data
Camera camera(camera_settings, glm::vec3(0.0, 1.0, 0.0));

double lastX = camera_settings.screenWidth / 2.0f;
double lastY = camera_settings.screenHeight / 2.0f;

#pragma region Light Related Stuff

// Holds information about the light to be rendered
struct Light {
	glm::vec4 ambient;
	glm::vec4 position;
	glm::vec4 colour;

	Light(glm::vec4 amb, glm::vec4 pos, glm::vec4 col) : ambient(amb), position(pos), colour(col) {}
};

// ========= AMBIENT STRENGTHS =========
glm::vec4 weakAmbient(1.0, 1.0, 1.0, 1.0);
glm::vec4 normalAmbient(5.0, 5.0, 5.0, 1.0);
glm::vec4 strongAmbient(10.0, 10.0, 10.0, 1.0);

// ========= LIGHT COLOURS =========
glm::vec4 white(1.0, 1.0, 1.0, 1.0);
glm::vec4 red(1.0, 0.0, 0.0, 1.0);
glm::vec4 green(0.0, 1.0, 0.0, 1.0);
glm::vec4 blue(0.0, 0.0, 1.0, 1.0);

#pragma endregion

#pragma region Model Functions

glm::mat4 defaultTranslate = glm::translate(glm::mat4(1.0), glm::vec3(0.0, 0.0, 0.0));
//glm::mat4 defaultRotate = glm::rotate(glm::mat4(1.0), 0.0, glm::vec3(1.0, 1.0, 1.0));
glm::mat4 defaultScale = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0));

// This struct holds information about the model to be rendered.
// Use this to render an object instead of rendering it directly from the model.
struct Object {
	Model model;
	glm::mat4 translation;
	// glm::mat4 rotation;
	glm::mat4 scale;

	Object(Model m, glm::mat4 t = defaultTranslate, /*glm::mat4, */ glm::mat4 s = defaultScale) :
		model(m), translation(t), scale(s) {}

	// New translation, rotation and scale are passed as paramaters for hierarchical purposes.
	void Render(GLuint shader, glm::mat4 t = defaultTranslate, glm::mat4 s = defaultScale) {
		glm::mat4 myModelMat = translation * scale * t * s;
		//if (t != defaultTranslate) myModelMat *= t;
		//if (s != defaultScale) myModelMat *= s;
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(myModelMat));
		model.draw(shader); //Draw the plane
	}
};

void RenderObject(Object object, GLuint shader) {
	object.Render(shader);
}

void RenderObject(Object objects[], int numOfItems, GLuint shader, glm::mat4 t = defaultTranslate, glm::mat4 s = defaultScale) {
	for (int i = 0; i < numOfItems; i++) {
		objects[i].Render(shader, t, s);
	}
}

#pragma endregion


int main()
{
	// glfw: initialize and configure
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);


	// glfw window creation
	GLFWwindow* window = glfwCreateWindow(camera_settings.screenWidth, camera_settings.screenHeight, "17123437 - Computer Graphics CW2", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set the callback functions
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);
	glfwSetKeyCallback(window, key_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//Rendering settings
	glfwSwapInterval(1);		// glfw enable swap interval to match screen v-sync
	glEnable(GL_DEPTH_TEST);	//Enables depth testing
	glEnable(GL_CULL_FACE);		//Enables face culling
	glFrontFace(GL_CCW);		//Specifies which winding order if front facing

	// ======================================= SHADER =======================================

	GLuint basicShader;

	// build and compile our shader program
	GLSL_ERROR glsl_err = ShaderLoader::createShaderProgram(
		string("Resources\\Shaders\\Basic_shader.vert"), 
		string("Resources\\Shaders\\Basic_shader.frag"),
		&basicShader);

	// ======================================= OBJECTS =======================================

	Model turf("Resources\\Models\\Turf\\turfWithMat.obj");						// Load the model
	glm::mat4 turfScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(5, 5, 5));	// Set up translation/rotation/scale
	Object turfObj(turf, defaultTranslate, turfScaleMat);						// Initialize the object

	Model stands("Resources\\Models\\Stands\\Stands.obj");											// Load the model
	glm::mat4 standsTranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -2.5, 0.0));		// Set up translation/rotation/scale
	glm::mat4 standsScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(9.2, 10, 10));					//
	Object standsObj(stands, standsTranslationMat, standsScaleMat);									// Initialize the object

	glm::mat4 stadiumScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));	// Set up translation/rotation/scale
	Object stadium[] = { turfObj, standsObj };											// Initialize the object
	

	// ======================================= LIGHTS =======================================

	Light l1(normalAmbient, glm::vec4(0.0, 8.0, 0.0, 1.0), white);
	Light l2(weakAmbient, glm::vec4(20.0, 2.0, 0.0, 1.0), red);
	Light l3(weakAmbient, glm::vec4(-20.0, 2.0, 0.0, 1.0), blue);

	glm::vec4 light_ambients[] = { l1.ambient, l2.ambient, l3.ambient };
	glm::vec4 light_positions[] = { l1.position, l2.position, l3.position };
	glm::vec4 light_colours[] = { l1.colour, l2.colour, l3.colour };

	GLfloat	attenuation[] = { 1.0, 0.10, 0.08 };

	// Materials
	GLfloat mat_amb_diff[] = { 1.0, 1.0, 1.0, 1.0 };	// Texture map will provide ambient and diffuse.
	GLfloat mat_specularCol[] = { 1.0, 1.0, 1.0, 1.0 }; // White highlight
	GLfloat mat_specularExp = 32.0;						// Shiny surface

	//======================================= BASIC SHADER UNIFORM LOCATION =======================================
	// Get unifom locations in shader
	GLuint lightAmbArr = glGetUniformLocation(basicShader, "lightAmbArray");
	GLuint lightPosArr = glGetUniformLocation(basicShader, "lightPosArray");
	GLuint lightColArr = glGetUniformLocation(basicShader, "lightColArray");
	GLuint uLightAttenuation = glGetUniformLocation(basicShader, "lightAttenuation");
	GLuint uEyePos = glGetUniformLocation(basicShader, "eyePos");

	// Get material unifom locations in shader
	GLuint uMatAmbient = glGetUniformLocation(basicShader, "matAmbient");
	GLuint uMatDiffuse = glGetUniformLocation(basicShader, "matDiffuse");
	GLuint uMatSpecularCol = glGetUniformLocation(basicShader, "matSpecularColour");
	GLuint uMatSpecularExp = glGetUniformLocation(basicShader, "matSpecularExponent");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		glm::vec3 eyePos = camera.getCameraPosition();

		glUseProgram(basicShader); //Use the Basic shader

		float lightSize = sizeof(light_colours) / sizeof(*light_colours);

		//Pass the uniform data to Basic shader///////////////////////////////////
		//Pass the light data
		glUniform4fv(lightColArr, lightSize, (GLfloat*)&light_colours);
		glUniform4fv(lightAmbArr, lightSize, (GLfloat*)&light_ambients);
		glUniform4fv(lightPosArr, lightSize, (GLfloat*)&light_positions);
		glUniform3fv(uLightAttenuation, 1, (GLfloat*)&attenuation);
		glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);

		//Pass material data
		glUniform4fv(uMatAmbient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatDiffuse, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(uMatSpecularCol, 1, (GLfloat*)&mat_specularCol);
		glUniform1f(uMatSpecularExp, mat_specularExp);

		glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

		// Render the Stadium
		RenderObject(stadium, 2, basicShader, defaultTranslate, stadiumScaleMat);

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

#pragma region Engine Stuff

float camSpeed = 2.0f;

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow *window)
{
	timer.updateDeltaTime();

	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
		camSpeed = 10.0f;
	else
		camSpeed = 2.0f;

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds() * camSpeed);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds() * camSpeed);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds() * camSpeed);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds() * camSpeed);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	glViewport(0, 0, width, height);
	camera.updateScreenSize(width, height);
}

// glfw: whenever the mouse moves, this callback is called
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	double xoffset = xpos - lastX;
	double yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
	{
		camera.processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

#pragma endregion