#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <Shader.h>
#include <Shapes.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

/*
	Perhaps not the best implementation for creating these objects, but should function well for the
	purpose of this assignment. Since I am simply creating a static scene, I do not need to worry about
	the dynamic nature of creating each objects by creating constructors, getters and setters and separate classes. I simply build
	each object here and create them in main to reduce code and clutter
*/

//Preliminary addition of shader to specify unique material properties

class SceneObjects {

	Shape Object;

public:

	void Crayon(Shader shader){
		Object.CreateCone(0.45, 0.14, 0.14, 60); //Top of Crayon
		Object.CreateCircle(0, 0.35, 0, 0.14, 360); // Base of Crayon Tip
		Object.CreateCircle(0, 0.59, 0, 0.046, 360); // Tip of Crayon
		Object.CreateCylinder(0.35f, 0.14, 60); //Base

		shader.setFloat("material.shininess", 128.0f);
	}

	void Basketball() {
		Object.CreateSphere(0.25, 40, 40);
	}
	void Book() {
		Object.CreateRecCube(0.75, 0.45, 0.1);
	}

	void Block() {
		Object.CreateCube(0.4, 0.3);
	}

	void Surface(Shader shader) {
		Object.CreateRecCube(3.5, 2.5, 0.02);

		shader.setFloat("material.shininess", 256.0f);
	}

	void Paper(Shader shader) {
		Object.CreateRectangle(1.3, 0.95);
		shader.setFloat("material.shininess", 256.0f);
	}

	void Light() {
		Object.CreateSphere(0.25, 40, 40);
	}

};
