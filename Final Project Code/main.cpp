#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <string>
#include <Shader.h>
#include <Shapes.h>
#include <SceneObjects.h>

#define _USE_MATH_DEFINES
#include <math.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

const GLfloat HEIGHT = 600, WIDTH = 800;

GLuint VAO;
GLuint texture[7];

// Call back methods to help with screen adjustments and mouse, keyboard and scroll inputs and loading textures to shader
void FrameBufferCallBack(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
void mouseCallback(GLFWwindow* window, double xPosi, double yPosi);
void scrollCallback(GLFWwindow* window, double speedChange, double noChange);
void loadTexture(const char* filename, GLuint& textureId, Shader shader, GLuint texCount);
void createKeyLight(Shader shader);
void createPointLight(Shader shader, std::string shadePos, glm::vec3 position);

// camera
// Globle variables for camera - used in glm lookup
glm::vec3 cameraPos = glm::vec3(0.0, 1.5, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0, 0.0, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

//Mouse movement for camera
bool firstMouse = true;
float yaw = -90.0f;	// left/right movement used for mouse rotation of the camera
float pitch = 0.0f; // up/down movement used for the mouse rotation of the camera
float lastX = 800.0f / 2.0; 
float lastY = 600.0 / 2.0;
float fov = 45.0f; //Field of View

// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
float frameSpeed = 2.5; // speed the camera moves in the world

//Ortho/Perspective switch
bool perspectOn = true;

//Created a new class of objects so I can have each in one space
SceneObjects surface, crayon, basketball, block, testLight, testPLight, paper, book;

//Separate postions and rotations for each object
glm::vec3 objectPositions[]{
	glm::vec3(0.0f, 0.0f, -0.2f), //surface 
	glm::vec3(0.12f, 0.38f, -0.8f), //Crayon
	glm::vec3(0.3, 0.25f, 0.7f),  //basketball
	glm::vec3(-0.3, 0.0f, -0.5f), //block

	//TestLights to get position of lighting
	glm::vec3(0.0f, 1.5f, 2.5f), //testLight
	glm::vec3(0.0f, 2.5f, -2.5f), //testPlight
	glm::vec3(0.0, 0.024f, -0.7f), //Paper
	glm::vec3(0.0, 0.024f, -0.3f) // Book
};

//Each object will be rotated at different degrees/radians
float radPos[]{
	0.0f, //Surface
	75.0f,   //Crayon
	80.0f,  //basketball
	-30.0f, //block

	//TestLights to get position of lighting
	0.0f, //testLight
	0.0f, //testLight
	-60.0f, //paper
	95.0f
};

//Used to search for specific object to be placed in the below for loop
std::string objectList[]{
	"surface", "crayon", "basketball", "block", "testl", "testl2", "paper", "book"
};

//list of loaded textures
const char* textureFiles[]{
	"CrayonPic.jpg",     
	"CrayonLabel.jpg",
	"Block/block.jpg",
	"Table2.jpg",
	"Paper.jpg",
	"BookCover.jpg"
};

int main() {
	//initialize GLFW window
	glfwInit();

	//Declare version to initiate
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Window creation and error handling
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "OpenGL Circle Test", NULL, NULL);

	if (!window)
	{
		printf("Failed to create the Window");
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, FrameBufferCallBack);

	//Mouse callback methods
	glfwSetCursorPosCallback(window, mouseCallback);
	glfwSetScrollCallback(window, scrollCallback);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); //disables and hides cursor for camera movement

	//Load function pointers using GLAD
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		printf("Failed to intsantiate glad");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Shader newShader("vertexShaderSource.vs", "fragmentShaderSource.fs");

	//Set material properties to textures
	newShader.use();
	newShader.setInt("material.diffuse", 0);
	newShader.setInt("material.specular", 0);

	/*loadTexture("CrayonPic.jpg", texture[0], newShader, 1);
	loadTexture("Block/block.jpg", texture[1], newShader, 1);
	loadTexture("Table2.jpg", texture[2], newShader, 1);
	loadTexture("CrayonLabel.jpg", texture[3], newShader, 2);*/

	// a for loop to achieve same outcome as above, this times sorts through list of files
	// sizeof() function not working for this loop - hard set list size
	for (int i = 0; i < 6; i++) {
		unsigned int texCount = 1;
		//used to set to 2nd uniform variable to provide layering
		if (textureFiles[i] == "CrayonLabel.jpg") {
			texCount = 2;
		}
		loadTexture(textureFiles[i], texture[i], newShader, texCount);
	}

	while (!glfwWindowShouldClose(window))
	{
		//This helps move the camera and update position
		float currentFrame = static_cast<float>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		// Method for keyPresses
		processInput(window);

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		//Open Shader Object
		newShader.use();
		newShader.setVec3("viewPos", cameraPos);

		//Create Key and Point lights
		createKeyLight(newShader);
		createPointLight(newShader, "0", glm::vec3(0.0f, 1.5f, -2.0f));
		createPointLight(newShader, "1", glm::vec3(0.0f, 0.5f, 0.0f));
		
		//Loop to set postions of each object based on list container
		//comment out initial rotate matrix and uncomment other rotate matrix to get rotation about the x-axis to better view the object
		for (int i = 0; i < sizeof(objectPositions); i++) {
			//model
			glm::mat4 model(1.0f); // make sure to initialize matrix to identity matrix first
			model = glm::scale(model, glm::vec3(2.0f, 2.0f, 2.0f));

			model = glm::rotate(model, glm::radians(radPos[i]), glm::vec3(0.0f, 1.0f, 0.0f)); //all other objects, the orientation changes amongst the same y value.

			//Positions each object
			model = glm::translate(model, objectPositions[i]);
			newShader.setMat4("model", model); //Begin using methods in Shader to implement uniform matrices

			//Creates the crayon object and sets it to postition based on objectPostions[0]
			//Perhaps a switch could be better implemented here?
			if (objectList[i] == "crayon") {
				//activates and binds first and 2nd textures to crayon
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture[0]);
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, texture[1]);
				crayon.Crayon(newShader);
				//need to unbind the texture in this position otherwise it will implement on all objects
				glActiveTexture(GL_TEXTURE1);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			if (objectList[i] == "basketball") {
				basketball.Basketball();
			}
			if (objectList[i] == "block") {
				//Activates and binds texture in 3rd postion of list. 
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture[2]);

				block.Block();

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			if (objectList[i] == "surface") {
				//activates and binds last texture in texture list
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture[3]);
				surface.Surface(newShader);
			
			}
			if (objectList[i] == "paper") {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture[4]);
				paper.Paper(newShader);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			if (objectList[i] == "book") {
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture[5]);
				book.Book();
			}
			//Test lights
			/*if (objectList[i] == "testl") {
				testLight.Light();
			}
			if (objectList[i] == "testl2") {
				testPLight.Light();
			}*/

		};

		//view
		//The view matrix sets the camera position, the forward view of the camera, and the y - axis orientation of the camera (if negative, the image will be flipped)
		glm::mat4 view(1.0f);
		view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
		newShader.setMat4("view", view);

		//projection
		/*
			Conditional to send desired projection to the vertexShader, upon P key press the switch changes to true or false.
			There is a glitch in this logic as it can take multiple key presses to intialize switch. It looks as though it would like 
			to swap projections each time, but does not always transition in the desired manner
		*/
		glm::mat4 projection(1.0f);
		if (perspectOn == true) {
			projection = glm::perspective(glm::radians(55.0f), WIDTH / HEIGHT, 0.1f, 150.0f);
		}
		else {
			projection = glm::ortho(-5.0f, 5.0f, -5.0f, 5.0f, 0.1f, 100.0f);	
		}
		newShader.setMat4("projection", projection);

		glBindVertexArray(VAO);	 

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	return 0;

}

void FrameBufferCallBack(GLFWwindow* window, int width, int height) {
	//Create viewport in the created window to draw to
	glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
	// closes window based on escape key press
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	//Camera view orthographic vs projection
	/*
	* This conditional seems to be a little glitchy - it can take multiple
	* key presses to get to desired state. 
	*/
	if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		if (perspectOn == false) {
			perspectOn = true; //turn on projection
		}
		else {
			perspectOn = false; //turn on orthographic
		}
	}
	float cameraSpeed = static_cast<float>(frameSpeed * deltaTime);
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraFront;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed; //uses cross product to get a normalized directional vector to move left or right
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		cameraPos += cameraSpeed * cameraUp;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		cameraPos -= cameraSpeed * cameraUp;

}
void mouseCallback(GLFWwindow* window, double xPosi, double yPosi) {
	float xpos = static_cast<float>(xPosi);
	float ypos = static_cast<float>(yPosi);

	//Initializes the first actions used by the mouse to prevent undesired movement at startup
	if (firstMouse) 
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastX;
	float yoffset = lastY - ypos; 
	lastX = xpos;
	lastY = ypos;

	//Mouse movement sensitivity
	float sensitivity = 0.1f; 
	yoffset *= sensitivity;

	yaw += xoffset;
	pitch += yoffset;

	// Keeps the model view in the correct orientation, if it were to leave these bounds object will flip
	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	/*
		Adjusts the camera view based on Euler angles.
		The yaw angles of x and z are influences by the pitch angle of the camera
		Therefore we need to multiply by the adjacent pitch angle for the resultant x and z
	*/
	glm::vec3 front;
	front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
	front.y = sin(glm::radians(pitch));
	front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
	cameraFront = glm::normalize(front); //normalize is purely directional and disregards the magnetude of the vector. This helps orient where the camera points in the world
}
void scrollCallback(GLFWwindow* window, double xScroll, double yScroll)
{
	//creates speed limits for movement in the world
	//Scroll up - slows the movement
	//scroll down - speeds up the movement
	//framespeed starts at 2.5f
	frameSpeed -= (float)yScroll;
	if (frameSpeed < 0.5f) frameSpeed = 0.5f;
	if (frameSpeed > 4.0f) frameSpeed = 4.0f;
}

//Texture count added to provide positioning of layered textures on objects
void loadTexture(const char* filename, GLuint& texture, Shader shader, GLuint texCount) {
	
	//Generate and bind the texture values for the shader to implement
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	// set the texture wrapping parameters - S and T are similar to X and Y values but for wrapping the texture coordingates.
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// Default wrapping for texture to GL_REPEAT if texture coordinates out of bound (0 - 1)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);//GL_Linear provides a weighted average of surrounding texture coord to generate cleaner image
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load image data to create texture and generate mipmaps
	int texWidth, texHeight, nrChannels;

	unsigned char* data = stbi_load(filename, &texWidth, &texHeight, &nrChannels, 0);
	if (data)
	{
		//glTexImage2D(Target texture type, 0 (base level of detail), Integer internal format, image width, height, legacy value (no longer used) set 0, reg format, data type, pointer)
		//Format will change based on image type such as Jpeg vs PNG. PNG requires GL_RGBA integer constant. 
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texWidth, texHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D); //Mipmap helps with quality of resolution and reduce impact on GPU here set as 2D image
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	shader.use();

	//I have set up to 4 possible texture layers to be implemented, however shader only implements up to two
	//wont be able to use mix method to implement more the 2 I believe. 
	switch (texCount) {
	case 1:
		shader.setInt("ourTexture", 0);
		break;
	case 2:
		shader.setInt("ourTexture2", 1);
		break;
	case 3:
		shader.setInt("ourTexture3", 2);
		break;
	case 4:
		shader.setInt("ourTexture4", 3);
		break;
	}

}

//Key/Diffuse/directional light
void createKeyLight(Shader shader) {
	shader.use();
	//Key Light
	shader.setVec3("dirLight.ambient", glm::vec3(1.0f, 1.0f, 1.0f)); //Bright scene
	shader.setVec3("dirLight.diffuse", glm::vec3(2.0f, 2.0f, 2.0f)); //Full light
	shader.setVec3("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f)); //Reflective properties
	shader.setVec3("dirLight.direction", glm::vec3(0.0f, -6.5f, 4.0f)); //light position/direction
}

//Secondary Light initially positioned behind the scene
void createPointLight(Shader shader, std::string shadePos, glm::vec3 position) {
	shader.use();

	//fill Light
	shader.setVec3("pointLights[" + shadePos + "].position", position);
	shader.setVec3("pointLights[" + shadePos + "].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	shader.setVec3("pointLights[" + shadePos + "].diffuse", glm::vec3(1.0f, 1.0f, 0.0f));
	shader.setVec3("pointLights[" + shadePos + "].specular", glm::vec3(0.5f, 0.5f, 0.5f));
	shader.setFloat("pointLights[" + shadePos + "].constant", 1.0f);
	shader.setFloat("pointLights[" + shadePos + "].linear", 0.09f);
	shader.setFloat("pointLights[" + shadePos + "].quadratic", 0.032f);
}
