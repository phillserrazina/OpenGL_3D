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
Camera camera(camera_settings, glm::vec3(00.0, 0.1, -4.0));

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
glm::vec4 superWeakAmbient(0.1, 0.1, 0.1, 1.0);
glm::vec4 weakAmbient(1.0, 1.0, 1.0, 1.0);
glm::vec4 normalAmbient(5.0, 5.0, 5.0, 1.0);
glm::vec4 strongAmbient(10.0, 10.0, 10.0, 1.0);

glm::vec4 weakBlueAmbient(0.3, 0.3, 1.0, 1.0);
glm::vec4 normalBlueAmbient(0.3, 0.3, 3.0, 1.0);
glm::vec4 strongBlueAmbient(0.0, 0.0, 5.0, 1.0);

glm::vec4 weakRedAmbient(1.0, 0.3, 0.3, 1.0);
glm::vec4 normalRedAmbient(3.0, 0.3, 0.3, 1.0);
glm::vec4 strongRedAmbient(5.0, 0.0, 0.0, 1.0);

// ========= LIGHT COLOURS =========
glm::vec4 white(1.0, 1.0, 1.0, 1.0);
glm::vec4 red(1.0, 0.0, 0.0, 1.0);
glm::vec4 green(0.0, 1.0, 0.0, 1.0);
glm::vec4 blue(0.0, 0.0, 1.0, 1.0);
glm::vec4 dark_blue(0.0, 0.0, 0.7, 1.0);

#pragma endregion

#pragma region Material Related Stuff
enum Materials { WOOD, BRASS, GRASS, LEATHER, NONE };

struct MaterialUniforms
{
	GLuint ambient;
	GLuint diffuse;
	GLuint specular;
	GLuint exponent;
};

MaterialUniforms matU;

void useMaterial(Materials, MaterialUniforms);
#pragma endregion

#pragma region Model Functions

// Default variables for easy access
glm::mat4 defaultTranslate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0, 0.0, 0.0f));
glm::mat4 defaultRotate = glm::rotate(glm::mat4(1.0f), glm::radians(0.0f), glm::vec3(1.0f, 1.0f, 1.0f));
glm::mat4 defaultScale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0, 1.0, 1.0f));

// This struct holds information about the model to be rendered.
// Use this to render an object instead of rendering it directly from the model.
struct Object {
	Model model;
	Materials material;
	glm::mat4 translation;
	glm::mat4 rotation;
	glm::mat4 scale;

	Object(Model m, Materials mat, glm::mat4 t = defaultTranslate, glm::mat4 r = defaultRotate, glm::mat4 s = defaultScale) :
		model(m), material(mat), translation(t), rotation(r), scale(s) {}

	// New translation, rotation and scale are passed as paramaters for hierarchical purposes.
	void Render(GLuint shader, glm::mat4 t = defaultTranslate, glm::mat4 r = defaultRotate, glm::mat4 s = defaultScale) {
		glm::mat4 myModelMat = translation * rotation * scale * t * r * s;
		useMaterial(material, matU);
		glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(myModelMat));
		model.draw(shader); //Draw the plane
	}
};

// Function Prototypes
void RenderObject(Object object, GLuint shader, glm::mat4 t = defaultTranslate, glm::mat4 r = defaultRotate, glm::mat4 s = defaultScale);
void RenderObject(Object objects[], int numOfItems, GLuint shader, glm::mat4 t = defaultTranslate, glm::mat4 r = defaultRotate, glm::mat4 s = defaultScale);

#pragma endregion

#pragma region Movement Utilities
static float t = 0.0f;
float infoDisplayTimer = 0.0f;

float playerMovX = 0;
float playerMovY = 1;
float playerMovZ = 0;

float playerSpeed = 2;

float playerRotY = -90.0f;
float playerRotX = 0.0f;
float playerRotZ = 0.0f;

float ballsRotY = 0.0f;

float lightPosX = 0.0;
float lightPosZ = -6.5;
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

	Model turf("Resources\\Models\\Turf\\turfWithMat.obj");												// Load the model
	glm::mat4 turfScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(5, 5, 5));							// Set up translation/rotation/scale
	Object turfObj(turf, GRASS, defaultTranslate, defaultRotate, turfScaleMat);							// Initialize the object

	Model stands("Resources\\Models\\Stands\\Stands.obj");												// Load the model
	glm::mat4 standsTranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -2.5, 0.0));			// Set up translation/rotation/scale
	glm::mat4 standsScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(9.2, 10, 10));						//
	Object standsObj(stands, WOOD, standsTranslationMat, defaultRotate, standsScaleMat);				// Initialize the object

	glm::mat4 stadiumScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));					// Set up translation/rotation/scale
	Object stadium[] = { turfObj, standsObj };															// Initialize the object
	
	Model character("Resources\\Models\\Character\\Player_OpenGL.obj");									// Load the model
	glm::mat4 characterScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.5, 0.5, 0.5));					// Set up scale
	Object characterObj(character, NONE, defaultTranslate, defaultRotate, characterScaleMat);			// Initialize the object

	Model chest("Resources\\Models\\Chest\\Chest.obj");													// Load the model
	glm::mat4 chestTranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -9.5, -8.2));			// Set up translation
	glm::mat4 chestScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(10.0, 10.0, 10.0));					// Set up scale
	Object chestObj(chest, WOOD, chestTranslationMat, defaultRotate, chestScaleMat);					// Initialize the object

	Model quaffle("Resources\\Models\\Balls\\Quaffle_V2.obj");											// Load the model
	glm::mat4 quaffleTranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(0.0, -0.1, -8.0));		// Set up translation
	glm::mat4 quaffleScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(10.0, 10.0, 10.0));				// Set up scale
	Object quaffleObj(quaffle, LEATHER, quaffleTranslationMat, defaultRotate, quaffleScaleMat);			// Initialize the object

	Model bludger("Resources\\Models\\Balls\\Bludger.obj");												// Load the model
	glm::mat4 bludger1TranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(2.0, 0.0, -20.0));		// Set up translation
	glm::mat4 bludger2TranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(-2.0, 0.0, -20.0));		// Set up translation
	glm::mat4 bludgerScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(0.4, 0.4, 0.4));					// Set up scale
	Object bludgerObj(bludger, BRASS, defaultTranslate, defaultRotate, bludgerScaleMat);				// Initialize the object

	Model hoops("Resources\\Models\\Hoops\\Hoops.obj");													// Load the model
	glm::mat4 hoopsScaleMat = glm::scale(glm::mat4(1.0), glm::vec3(1.0, 1.0, 1.0));						// Set up scale
	glm::mat4 hoops1TranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(17.0, -2.0, 0.0));		// Set up translation
	glm::mat4 hoops2TranslationMat = glm::translate(glm::mat4(1.0), glm::vec3(-17.0, -2.0, 0.0));		// Set up translation
	Object hoopsObj(hoops, BRASS, defaultTranslate, defaultRotate, hoopsScaleMat);						// Initialize the object

	// ======================================= LIGHTS =======================================

	Light l1(weakAmbient, glm::vec4(0.0, 8.0, 0.0, 1.0), white);
	Light l2(normalRedAmbient, glm::vec4(20.0, 2.0, 0.0, 1.0), red);
	Light l3(normalBlueAmbient, glm::vec4(-20.0, 2.0, 0.0, 1.0), blue);

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
	GLuint lightTypeArr = glGetUniformLocation(basicShader, "lightTypeArray");
	GLuint uLightAttenuation = glGetUniformLocation(basicShader, "lightAttenuation");
	GLuint uEyePos = glGetUniformLocation(basicShader, "eyePos");

	// Get material unifom locations in shader
	matU.ambient = glGetUniformLocation(basicShader, "matAmbient");
	matU.diffuse = glGetUniformLocation(basicShader, "matDiffuse");
	matU.specular = glGetUniformLocation(basicShader, "matSpecularColour");
	matU.exponent = glGetUniformLocation(basicShader, "matSpecularExponent");

	// render loop
	while (!glfwWindowShouldClose(window))
	{
		// input
		processInput(window);
		timer.tick();

		// render
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glm::mat4 model = glm::mat4(1.0);
		glm::mat4 view = camera.getViewMatrix();
		glm::mat4 projection = camera.getProjectionMatrix();
		glm::vec3 eyePos = camera.getCameraPosition();
		glm::mat4 scale = glm::scale(glm::mat4(1.0), glm::vec3(0.01, 0.01, 0.01));
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0), glm::radians(12.0f), glm::vec3(0.0, 1.0, 0.0));

		glUseProgram(basicShader); //Use the Basic shader

		Light l4(superWeakAmbient, glm::vec4(lightPosX, 2.0, lightPosZ, 1.0), white);

		glm::vec4 light_ambients[] = { l1.ambient, l2.ambient, l3.ambient, l4.ambient };
		glm::vec4 light_positions[] = { l1.position, l2.position, l3.position, l4.position };
		glm::vec4 light_colours[] = { l1.colour, l2.colour, l3.colour, l4.colour };

		float lightSize = sizeof(light_colours) / sizeof(*light_colours);

		//Pass the uniform data to Basic shader///////////////////////////////////
		//Pass the light data
		glUniform4fv(lightColArr, lightSize, (GLfloat*)&light_colours);
		glUniform4fv(lightAmbArr, lightSize, (GLfloat*)&light_ambients);
		glUniform4fv(lightPosArr, lightSize, (GLfloat*)&light_positions);
		glUniform3fv(uLightAttenuation, 1, (GLfloat*)&attenuation);
		glUniform3fv(uEyePos, 1, (GLfloat*)&eyePos);

		//Pass material data
		glUniform4fv(matU.ambient, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(matU.diffuse, 1, (GLfloat*)&mat_amb_diff);
		glUniform4fv(matU.specular, 1, (GLfloat*)&mat_specularCol);
		glUniform1f(matU.exponent, mat_specularExp);

		glUniformMatrix4fv(glGetUniformLocation(basicShader, "view"), 1, GL_FALSE, glm::value_ptr(view));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
		glUniformMatrix4fv(glGetUniformLocation(basicShader, "model"), 1, GL_FALSE, glm::value_ptr(model));

		GLint lightDirectionLoc = glGetUniformLocation(basicShader, "lightDirection");
		GLint lightDiffuseLoc = glGetUniformLocation(basicShader, "lightDiffuseColour");
		glUniform4f(lightDirectionLoc, 1.0f, 1.0f, 0.5f, 0.0f);
		glUniform4f(lightDiffuseLoc, 1.0f, 1.0f, 1.0f, 1.0f);

		// Render the Stadium
		RenderObject(stadium, 2, basicShader, defaultTranslate, defaultRotate, stadiumScaleMat);


		RenderObject(chestObj, basicShader);
		RenderObject(hoopsObj, basicShader, hoops1TranslationMat);
		RenderObject(hoopsObj, basicShader, hoops2TranslationMat);
		
		ballsRotY += 1.0;
		glm::mat4 ballsRotMat = glm::rotate(glm::mat4(1.0), glm::radians(ballsRotY), glm::vec3(0.0, 1.0, 0.0));
		RenderObject(quaffleObj, basicShader, defaultTranslate, ballsRotMat);
		RenderObject(bludgerObj, basicShader, bludger1TranslationMat, ballsRotMat);
		RenderObject(bludgerObj, basicShader, bludger2TranslationMat, ballsRotMat);

		glm::vec3 playerMovPos = glm::vec3(playerMovX * playerSpeed, playerMovY * playerSpeed, playerMovZ * playerSpeed);
		glm::mat4 playerMovMat = glm::translate(glm::mat4(1.0), playerMovPos);
		glm::mat4 playerRotMatX = glm::rotate(glm::mat4(1.0), glm::radians(playerRotX), glm::vec3(1.0, 0.0, 0.0));
		glm::mat4 playerRotMatY = glm::rotate(glm::mat4(1.0), glm::radians(playerRotY), glm::vec3(0.0, 1.0, 0.0));
		glm::mat4 playerRotMatZ = glm::rotate(glm::mat4(1.0), glm::radians(playerRotZ), glm::vec3(0.0, 0.0, 1.0));
		glm::mat4 playerRotMat = playerRotMatX * playerRotMatY * playerRotMatZ;
		// Render the player
		RenderObject(characterObj, basicShader, playerMovMat, playerRotMat);

		if (camera.getState() == FOLLOWING)
			camera.followPosition(glm::vec3(playerMovX, playerMovY, playerMovZ), glm::vec3(5.0, 2.0, 0.0), glm::vec2(180.0, -20.0));

		//Static time
		t += timer.getDeltaTimeSeconds();

		// Display info every second
		infoDisplayTimer += timer.getDeltaTimeSeconds();
		if (infoDisplayTimer >= 1.0f) {
			cout << "Time: " << (int)t << "; FPS: " << (int)(1.0 / timer.getDeltaTimeSeconds()) << ";" << endl;
			infoDisplayTimer = 0.0f;
		}
		

		// glfw: swap buffers and poll events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// glfw: terminate, clearing all previously allocated GLFW resources.
	glfwTerminate();
	return 0;
}

#pragma region Function Implementations

void RenderObject(Object object, GLuint shader, glm::mat4 t, glm::mat4 r, glm::mat4 s ) {
	object.Render(shader, t, r, s);
}

void RenderObject(Object objects[], int numOfItems, GLuint shader, glm::mat4 t, glm::mat4 r, glm::mat4 s) {
	for (int i = 0; i < numOfItems; i++) {
		objects[i].Render(shader, t, r, s);
	}
}

void useMaterial(Materials chosenMaterial, MaterialUniforms matVar) {
	switch (chosenMaterial)
	{
	case WOOD:
		glUniform4fv(matVar.ambient, 1, glm::value_ptr(glm::vec3(0.32, 0.22, 0.07)));
		glUniform4fv(matVar.diffuse, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glUniform4fv(matVar.specular, 1, glm::value_ptr(glm::vec3(0.7, 0.6, 0.6)));
		glUniform1f(matVar.exponent, 2);

	case BRASS:
		glUniform4fv(matVar.ambient, 1, glm::value_ptr(glm::vec3(0.32, 0.22, 0.07)));
		glUniform4fv(matVar.diffuse, 1, glm::value_ptr(glm::vec3(0.78, 0.36, 0.22)));
		glUniform4fv(matVar.specular, 1, glm::value_ptr(glm::vec3(0.992157, 0.941176, 0.507043)));
		glUniform1f(matVar.exponent, 120.89743616);

	case GRASS:
		glUniform4fv(matVar.ambient, 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glUniform4fv(matVar.diffuse, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glUniform4fv(matVar.specular, 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glUniform1f(matVar.exponent, 1);

	case LEATHER:
		glUniform4fv(matVar.ambient, 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glUniform4fv(matVar.diffuse, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glUniform4fv(matVar.specular, 1, glm::value_ptr(glm::vec3(0.1, 0.1, 0.1)));
		glUniform1f(matVar.exponent, 50.0);

	case NONE:
		glUniform4fv(matVar.ambient, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glUniform4fv(matVar.diffuse, 1, glm::value_ptr(glm::vec3(1.0, 0.0, 0.0)));
		glUniform4fv(matVar.specular, 1, glm::value_ptr(glm::vec3(1.0, 1.0, 1.0)));
		glUniform1f(matVar.exponent, 1.0);
	}
}

#pragma endregion

#pragma region Engine Stuff

float camSpeed = 2.0f;
bool isPressingRotate = false;
bool isPressingFollow = false;

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

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (camera.getState() == FREE)
			camera.processKeyboard(FORWARD, timer.getDeltaTimeSeconds() * camSpeed);
		else if (camera.getState() == FOLLOWING) {
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				playerMovY += 0.1;
			else
				playerMovX -= playerRotY == 90.0f ? -0.1 : 0.1;
		}
			
	}

	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (camera.getState() == FREE)
			camera.processKeyboard(BACKWARD, timer.getDeltaTimeSeconds() * camSpeed);
		else if (camera.getState() == FOLLOWING)
			if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
				playerMovY -= 0.1;
			else
				playerMovX += playerRotY == 90.0f ? -0.1 : 0.1;
	}

	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		if (camera.getState() == FREE)
			camera.processKeyboard(LEFT, timer.getDeltaTimeSeconds() * camSpeed);
		else if (camera.getState() == FOLLOWING)
			playerMovZ += playerRotY == 90.0f ? -0.05 : 0.05;
	}

	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (camera.getState() == FREE)
			camera.processKeyboard(RIGHT, timer.getDeltaTimeSeconds() * camSpeed);
		else if (camera.getState() == FOLLOWING)
			playerMovZ -= playerRotY == 90.0f ? -0.05 : 0.05;
	}
		
	if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
		playerRotZ += 1.0;
	if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
		playerRotZ -= 1.0;
	if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			playerRotX -= 1.0;
		else
			playerRotY -= 1.0;
	}
		
	if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
		if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			playerRotX += 1.0;
		else
			playerRotY += 1.0;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS && !isPressingFollow) {
		camera.setState(camera.getState() == FREE ? FOLLOWING : FREE);
		isPressingFollow = true;
	}

	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_RELEASE) {
		isPressingFollow = false;
	}

	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !isPressingRotate) {
		playerRotY *= -1;
		isPressingRotate = true;
	}
	
	if (glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE)
		isPressingRotate = false;

	if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS)
		lightPosX += 0.05;

	if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS)
		lightPosX -= 0.05;

	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS)
		lightPosZ += 0.05;

	if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS)
		lightPosZ -= 0.05;
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
		if (camera.getState() == FREE)
			camera.processMouseMovement(xoffset, yoffset);
	}
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.processMouseScroll(yoffset);
}

#pragma endregion