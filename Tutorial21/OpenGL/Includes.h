#ifndef INCLUDES_H
#define INCLUDES_H
//Libraries
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
#include <chrono>
#include <string>
#include <sstream> 
#include <iostream>
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>


//Local files
#include "Timer.h"
#include "Model.h"
#include "Camera.h"
#include "SkinnedMesh.h"
#include "ShaderLoader.h"
#include "TextureLoader.h"

//namespaces
using std::string;

#endif