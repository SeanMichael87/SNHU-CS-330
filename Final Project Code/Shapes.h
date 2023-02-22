#pragma once
#include <stdio.h>
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <Shader.h>

#define _USE_MATH_DEFINES
#include <math.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

class Shape {

public:

	GLuint VAO, VBO, IBO;

	//Circle
		//Create Buffers for VAO, VBO, and IBO and create vertices for circle
	void CreateCircle(GLfloat x, GLfloat y, GLfloat z, GLfloat radius, GLint numSides) {

		//math to create a circle
		const int numVertices = numSides + 2;
		GLfloat twoPi = 2.0f * M_PI;
		GLfloat theta = twoPi / numSides;

		std::vector<GLfloat> circleVertices;

		//initial circle positions
		circleVertices.push_back(x);
		circleVertices.push_back(y);
		circleVertices.push_back(z);

		//color vertices
		circleVertices.push_back(0.21);
		circleVertices.push_back(0.71);
		circleVertices.push_back(0.52);

		//Normals
		circleVertices.push_back(0.0);
		circleVertices.push_back(1.0);
		circleVertices.push_back(0.0);

		for (int i = 0; i < numVertices; i++) {
			circleVertices.push_back(x + (radius * cos(i * theta))); //creates next vertice in x positon
			circleVertices.push_back(y);                             //flat disk, so y will remain the same
			circleVertices.push_back(z + (radius * sin(i * theta))); //creates next vertice in the z position

			//Color vertices
			circleVertices.push_back(0.21);
			circleVertices.push_back(0.71);
			circleVertices.push_back(0.52);

			//Lighting Normals (placed upwards since these are the only circles facing the light source)
			circleVertices.push_back(0.0);
			circleVertices.push_back(1.0);
			circleVertices.push_back(0.0);
		}


		//Generate VBO, VAO, and IBO
		//alot of this code is reused throughout this document, may need to think how to simplify
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, circleVertices.size() * sizeof(float), circleVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLE_FAN, 0, numVertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	void CreateRectangle(GLfloat baseLength, GLfloat baseWidth) {
		GLfloat x = baseLength / 2, z = baseWidth / 2;
		
		GLfloat rectangleVertices[]{
			//Positions		  //Colors                  //Texture     //Normal             
			
			-x, 0.0, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,	   //Top
			x, 0.0, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  0.0f, 1.0f, 0.0f,	   
			-x, 0.0, -z,	  1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   0.0f, 1.0f, 0.0f,	   
			-x, 0.0, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,	   
			x, 0.0, z,        1.0f, 1.0f, 1.0f,  	    1.0f, 0.0f,	  0.0f, 1.0f, 0.0f,	   
			x, 0.0, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  0.0f, 1.0f, 0.0f,	   

		
		};
		

		glEnable(GL_DEPTH_TEST);
		//Create the VAO 
		glGenVertexArrays(1, &VAO);
		//Binds the specific VAO
		glBindVertexArray(VAO);

		//Create VBO for inside VAO
		glGenBuffers(1, &VBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(rectangleVertices), rectangleVertices, GL_STATIC_DRAW);

		//Connect the vertices created to the VBO 

		//First takes location based on shader, then number
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		//color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute - set strides to 8 * sizeof to account for the texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//UnBind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Cylinder
	void CreateCylinder(GLfloat height, GLfloat radius, GLint numSides) {

		//Based on the logic of creating a circle I was able to create the cylinder by modifying 'y' vertice
		const int numVertices = 2 * (numSides + 2);
		GLfloat twoPi = 2.0f * M_PI;
		GLfloat theta = twoPi / numSides;
		//The coordinates of the texture will only be either 0 or 1 for the y position
		//Therefore, the onle main variable will be the X position
		GLfloat texX, normX, normZ;
	

		std::vector<GLfloat> cylinderVertices;

		for (int i = 0; i < numVertices; i++) {

			//Changes the position of x tex coordinate as the loop iterates
			//sets to number of sides on the cylinder (ex: 3rd vertice will be (2/numSides = x = 0.0333)) increments through the loop
			// i is and integer so need to cast to float to properly function
			texX = (float) i / numSides;
			normX = -cos(i * theta);
			normZ = -sin(i * theta);
			

			if (i % 2 == 1) {
				//upper postions of cylinder
				cylinderVertices.push_back(radius * cos(i * theta));
				cylinderVertices.push_back(height);
				cylinderVertices.push_back(radius * sin(i * theta));

				//Color vertices
				cylinderVertices.push_back(0.21);
				cylinderVertices.push_back(0.71);
				cylinderVertices.push_back(0.52);

				//texture
				cylinderVertices.push_back(texX);
				cylinderVertices.push_back(1.0f);

				//Seed Normal Vertices
				cylinderVertices.push_back(-cos(i * theta));
				cylinderVertices.push_back(0.0);
				cylinderVertices.push_back(-sin(i * theta));
			}
			else {
				//lower positions of cylinder
				cylinderVertices.push_back(radius * cos(i * theta));
				cylinderVertices.push_back(-height);
				cylinderVertices.push_back(radius * sin(i * theta));

				//Color vertices
				cylinderVertices.push_back(0.21);
				cylinderVertices.push_back(0.71);
				cylinderVertices.push_back(0.52);

				//texture
				cylinderVertices.push_back(texX);
				cylinderVertices.push_back(0.0f);

				//Seed Normal Vertices
				cylinderVertices.push_back(-cos(i*theta));
				cylinderVertices.push_back(0.0);
				cylinderVertices.push_back(-sin(i * theta));
			}
			
		}

		/*calcAverageNormals(cylinderVertices, sizeof(cylinderVertices), 11, 8, 33);*/

		// I am still trying to think my way on how to create the Index array for this shape

		//Generate VBO, VAO, and IBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute - set strides to 8 * sizeof to account for the texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);


		//Used triangle strip since I wouldn't have to worry as much about grouping vertices in sets of 3
		//It simply builds off the previous vertice to create the triangle mesh
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 120);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}
	//Cone
		/*
			Similar to the cylinder logic, but I reduce the radius at one end of the shape to achieve a cone.
			I will use this for creating my crayon in my project, so added a position to the height to help make
			it easier to create the shape.
		*/
	void CreateCone(GLfloat position, GLfloat height, GLfloat radius, GLint numSides) {

		const int numVertices = 2 * (numSides + 2);
		GLfloat twoPi = 2.0f * M_PI;
		GLfloat theta = twoPi / numSides;

		std::vector<GLfloat> coneVertices;
	
		for (int i = 0; i < numVertices; i++) {

			if (i % 2 == 0) {

				coneVertices.push_back(radius / 3 * cos(i * theta));
				coneVertices.push_back(height + position);
				coneVertices.push_back(radius / 3 * sin(i * theta));

				//Color vertices
				coneVertices.push_back(1.0);
				coneVertices.push_back(1.0);
				coneVertices.push_back(1.0);

				//Seed Normal Vertices
				coneVertices.push_back(-cos(i * theta));
				coneVertices.push_back(0.0);
				coneVertices.push_back(-sin(i * theta));
			}
			else {
				coneVertices.push_back(radius * cos(i * theta));
				coneVertices.push_back(-height + position);
				coneVertices.push_back(radius * sin(i * theta));

				//Color vertices
				coneVertices.push_back(0.21);
				coneVertices.push_back(0.71);
				coneVertices.push_back(0.52);

				//Seed Normal Vertices
				coneVertices.push_back(-cos(i*theta));
				coneVertices.push_back(0.0);
				coneVertices.push_back(-sin(i*theta));
			}

		}

		/*calcAverageNormals(coneVertices, numVertices, 9, 6, 27);*/


		//Generate VBO, VAO, and IBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, coneVertices.size() * sizeof(float), coneVertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLE_STRIP, 0, 120);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	//Sphere
		/*
			http://www.songho.ca/opengl/gl_sphere.html. The hyperlink here really helped me with the math for creating a sphere.
			I was able to think through and implement the circle, cylinder, and cone, but was struggling with the complexity of this shape.
			The website also showed how I can implement the indices and Draw Element command to draw the shape. He mentioned sector and stack count
			but I felt that longitude and latitude helped me better understand how to create the sphere.
		*/
	void CreateSphere(GLfloat radius, GLint longCount, GLint latCount) {

		GLfloat longStep = 2.0f * M_PI / longCount;
		GLfloat latStep = M_PI / latCount;
		GLfloat longAngle, latAngle, x, y, z, xyLat;

		std::vector<GLfloat> sphereVertices;

		for (int i = 0; i <= latCount; ++i)
		{
			latAngle = M_PI / 2.0 - i * latStep;        // starting from PI/2 to -PI/2
			xyLat = radius * cos(latAngle);
			z = radius * sin(latAngle);

			// the first and last vertices have same position
			for (int j = 0; j <= longCount; ++j)
			{
				longAngle = j * longStep;           // starting from 0 to 2*PI

				// vertex position (x, y, z)
				x = xyLat * cosf(longAngle);             // r * cos(u) * cos(v)
				y = xyLat * sinf(longAngle);             // r * cos(u) * sin(v)
				sphereVertices.push_back(x);
				sphereVertices.push_back(y);
				sphereVertices.push_back(z);

				if (j % 2 == 0) {
					sphereVertices.push_back(0.0f);
					sphereVertices.push_back(0.0f);
					sphereVertices.push_back(1.0f);

					//Normals
					sphereVertices.push_back(-cosf(longAngle));
					sphereVertices.push_back(sinf(longAngle)); 
					sphereVertices.push_back(-sinf(latAngle));
				}
				else {
					sphereVertices.push_back(0.0f);
					sphereVertices.push_back(1.0f);
					sphereVertices.push_back(0.0f);

					//Normals
					sphereVertices.push_back(-cosf(longAngle));
					sphereVertices.push_back(sinf(longAngle));
					sphereVertices.push_back(-sinf(latAngle));
				}

			}
		}

		/*
			I am still wrapping my head around creating the indices for these shapes
			so used the code created on songho website to create the array. All credit goes to them for this
			snippet of code
		*/

		std::vector<int> indices;
		int k1, k2;
		for (int i = 0; i < latCount; ++i)
		{
			k1 = i * (longCount + 1);
			k2 = k1 + longCount + 1;

			for (int j = 0; j < longCount; ++j, ++k1, ++k2)
			{
				if (i != 0)
				{
					indices.push_back(k1);
					indices.push_back(k2);
					indices.push_back(k1 + 1);
				}

				// k1+1 => k2 => k2+1
				if (i != (latCount - 1))
				{
					indices.push_back(k1 + 1);
					indices.push_back(k2);
					indices.push_back(k2 + 1);
				}

			}
		}

		//Generate VBO, VAO, and IBO
		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);

		glBindVertexArray(VAO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sphereVertices.size() * sizeof(float), sphereVertices.data(), GL_STATIC_DRAW);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(float), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawElements(GL_TRIANGLES, indices.size() * sizeof(unsigned int), GL_UNSIGNED_INT, 0);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

	}

	//Pyramid
		/*
			The pyramid is one of the basic shapes, so I created a simple array for this shape instead of a
			vector as it is not as dynamic as the others. I simply set the five positions and create the shape
		*/
	void CreatePyramid(GLfloat baseWidth, GLfloat height, bool outline) {

		GLfloat x = baseWidth / 2, z = baseWidth / 2;

		GLfloat pyVertices[]{
			//Positions					//Colors             //indice position
			//x		y		z
			x, 0.0f, 0.0f,         0.9f, 0.4f, 0.4f,  //0
			0.0f, height, 0.0f,	   0.5f, 1.0f, 0.5f,  //1
			0.0f, 0.0f, z,         1.0f, 0.51f, 0.2f, //2
			-x, 0.0f, 0.0f,        1.0f, 0.0f, 0.0f,  //3
			0.0f, 0.0f, -z,        0.0f, 0.0f, 1.0f   //4
		};

		//It is not as complex to think about the indices for this shape, I have labeled which create which
		//portion of the pyramid

		GLuint indices[] = {

		0,1,2, //Base to Peak (position 1)
		3,1,2,
		3,1,4,
		0,1,4,
		0,4,2, //Base
		3,4,2

		};

		glEnable(GL_DEPTH_TEST);
		//Create the VAO 
		glGenVertexArrays(1, &VAO);
		//Binds the specific VAO
		glBindVertexArray(VAO);

		//Create VBO for inside VAO
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &IBO);

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(pyVertices), pyVertices, GL_STATIC_DRAW);

		//Create Element/indices Buffer Object
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

		//Connect the vertices created to the VBO 

		//First takes location based on shader, then number
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		//color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute - set strides to 8 * sizeof to account for the texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);
		//Added a boolean to render two different pyramids based on this weeks assignment
		//If outline is true it will use line loop to create an outlined pyramid
		if (outline == true) {
			glDrawElements(GL_LINE_LOOP, 18, GL_UNSIGNED_INT, 0);
		}
		else {
			glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_INT, 0);
		}

		//UnBind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	//Cube
		/*
			Similar logic as to how i created the pyramid, it simply uses more vertices and traingles to create
			I also did not add the outline functionality to this as it was not required, it should be simple to
			implement if I want to
		*/
	void CreateCube(GLfloat baseWidth, GLfloat height) {

		GLfloat x = baseWidth / 2, z = baseWidth / 2 ;
		GLfloat y = height;

		GLfloat cubeVertices[]{
			//Positions					//Colors              //Texture      //Normal           //indice position
			0.0f, 0.0f, z,         1.0f, 0.51f, 0.5f,        0.0f, 0.0f,	0.0f, 0.0f, 1.0f,   //1 //Front    
			-x, 0.0f, 0.0f,        1.0f, 0.5f, 0.5f,         0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	//2
			0.0, y, z,             0.5f, 0.5f, 0.4f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	//5
			-x, 0.0f, 0.0f,        1.0f, 0.5f, 0.5f,         0.0f, 1.0f,	0.0f, 0.0f, 1.0f,	//2
			0.0, y, z,             0.5f, 0.5f, 0.4f,		 1.0f, 0.0f,	0.0f, 0.0f, 1.0f,	//5
			-x, y, 0.0f,            0.0f, 0.0f, 0.0f,        1.0f, 1.0f,	0.0f, 0.0f, 1.0f,	//11

			x, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,         0.0f, 1.0f,	0.0f, 0.0f, -1.0f,	//0 //Back
			0.0f, 0.0f, -z,        0.5f, 0.5f, 1.5f,         1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	//3
			x, y, 0.0f,	           0.5f, 0.5f, 0.5f,		 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	//4
			0.0f, 0.0f, -z,        0.5f, 0.5f, 1.5f,         1.0f, 1.0f,	0.0f, 0.0f, -1.0f,	//3
			0, y, -z,			   0.5f, 0.5f, 0.5f,		 1.0f, 0.0f, 	0.0f, 0.0f, -1.0f,	//10
			x, y, 0.0f,	           0.5f, 0.5f, 0.5f,		 0.0f, 0.0f,	0.0f, 0.0f, -1.0f,	//4

			x, y, 0.0f,	           0.5f, 0.5f, 0.5f,		 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	//4 //Top
			-x, y, 0.0,			   0.5f, 0.51f, 0.5f,		 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	//6
			0, y, -z,			   0.5f, 0.5f, 0.5f,		 1.0f, 1.0f, 	0.0f, 1.0f, 0.0f,	//7
			x, y, 0.0f,	           0.5f, 0.5f, 0.5f,		 0.0f, 0.0f,	0.0f, 1.0f, 0.0f,	//4
			0.0, y, z,             0.5f, 0.5f, 0.4f,		 1.0f, 0.0f,	0.0f, 1.0f, 0.0f,	//5
			-x, y, 0.0,			   0.5f, 0.51f, 0.5f,		 0.0f, 1.0f,	0.0f, 1.0f, 0.0f,	//6

			x, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,         0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//0 //Bottom
			0.0f, 0.0f, z,         1.0f, 0.51f, 0.5f,        0.0f, 0.0f,	0.0f, -1.0f, 0.0f,	//1          
			-x, 0.0f, 0.0f,        1.0f, 0.5f, 0.5f,         0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//2
			x, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,         0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//0
			-x, 0.0f, 0.0f,        1.0f, 0.5f, 0.5f,         0.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//2
			0.0f, 0.0f, -z,        0.5f, 0.5f, 1.5f,         1.0f, 1.0f,	0.0f, -1.0f, 0.0f,	//3

			-x, 0.0f, 0.0f,        1.0f, 0.5f, 0.5f,         0.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	//2 //Left
			0.0f, 0.0f, -z,        0.5f, 0.5f, 1.5f,         1.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	//3
			-x, y, 0.0f,            0.0f, 0.0f, 0.0f,        0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	//9
			0.0f, 0.0f, -z,        0.5f, 0.5f, 1.5f,         1.0f, 1.0f,	-1.0f, 0.0f, 0.0f,	//3
			-x, y, 0.0f,            0.0f, 0.0f, 0.0f,        0.0f, 0.0f,	-1.0f, 0.0f, 0.0f,	//9 
			0, y, -z,			   0.5f, 0.5f, 0.5f,		 1.0f, 0.0f, 	-1.0f, 0.0f, 0.0f,	//10

			x, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,         0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	//0 //Right
			x, y, 0.0f,            0.0f, 0.0f, 0.0f,         1.0f, 1.0f,	1.0f, 0.0f, 0.0f,	//8	
			0.0, y, z,             0.5f, 0.5f, 0.4f,		 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	//5
			x, 0.0f, 0.0f,         0.0f, 0.0f, 1.0f,         0.0f, 1.0f,	1.0f, 0.0f, 0.0f,	//0
			0.0f, 0.0f, z,         1.0f, 0.51f, 0.5f,        0.0f, 0.0f,	1.0f, 0.0f, 0.0f,	//1
			0.0, y, z,             0.5f, 0.5f, 0.4f,		 1.0f, 0.0f,	1.0f, 0.0f, 0.0f,	//5

		};

		/*GLuint indices[] = {*/

		//0,8,5, //Right
		//0,1,5,
		//1,2,5, //Front
		//2,5,11,
		//2,3,9, //Left
		//3,9,10,
		//0,3,4, //Back
		//3,10,4,
		//4,6,7, //Top
		//4,5,6,
		//0,1,2, //Bottom
		//0,2,3

	/*	};*/

		glEnable(GL_DEPTH_TEST);
		//Create the VAO 
		glGenVertexArrays(1, &VAO);
		//Binds the specific VAO
		glBindVertexArray(VAO);

		//Create VBO for inside VAO
		glGenBuffers(1, &VBO);
		/*glGenBuffers(1, &IBO);*/

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		//Create Element/indices Buffer Object
		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

		//Connect the vertices created to the VBO 

		//First takes location based on shader, then number
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		//color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute - set strides to 8 * sizeof to account for the texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//UnBind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Created a new object method to create surfaces without need to combine cubes
	void CreateRecCube(GLfloat baseLength, GLfloat baseWidth, GLfloat height) {

		GLfloat x = baseLength / 2, z = baseWidth / 2;
		GLfloat y = height;

		GLfloat cubeVertices[]{
			//Positions		  //Colors                  //Texture     //Normal             //indice position
			x, 0.0f, z,       1.0f, 1.0f, 1.0f,         1.0f, 0.0f,   0.0f, 0.0f, 1.0f,    //1   //Front       
			x, 0.0f, -z,      1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   0.0f, 0.0f, 1.0f,    //2
			x, y, z,          1.0f, 1.0f, 1.0f,  	    0.0f, 1.0f,	  0.0f, 0.0f, 1.0f,    //5
			x, 0.0f, -z,      1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   0.0f, 0.0f, 1.0f,    //2
			x, y, z,          1.0f, 1.0f, 1.0f,  	    0.0f, 1.0f,	  0.0f, 0.0f, 1.0f,    //5
			x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  0.0f, 0.0f, 1.0f,    //6

			-x, 0.0f, z,      1.0f, 1.0f, 1.0f,         0.0f, 0.0f,   0.0f, 0.0f, -1.0f,   //0 //Back
			-x, 0.0f, -z,     1.0f, 1.0f, 1.0f,         1.0f, 1.0f,   0.0f, 0.0f, -1.0f,   //3
			-x, y, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 0.0f, -1.0f,   //4
			-x, 0.0f, -z,     1.0f, 1.0f, 1.0f,         1.0f, 1.0f,   0.0f, 0.0f, -1.0f,   //3
			-x, y, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 0.0f, -1.0f,   //4
			-x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 0.0f,   0.0f, 0.0f, -1.0f,   //7

			-x, y, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,	   //4 //Top
			x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  0.0f, 1.0f, 0.0f,	   //6
			-x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 0.0f,   0.0f, 1.0f, 0.0f,	   //7
			- x, y, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	  0.0f, 1.0f, 0.0f,	   //4
			x, y, z,          1.0f, 1.0f, 1.0f,  	    0.0f, 1.0f,	  0.0f, 1.0f, 0.0f,	   //5
			x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  0.0f, 1.0f, 0.0f,	   //6

			-x, 0.0f, z,      1.0f, 1.0f, 1.0f,         0.0f, 0.0f,   0.0f, -1.0f, 0.0f,   //0 //Bottom
			x, 0.0f, z,       1.0f, 1.0f, 1.0f,         1.0f, 0.0f,   0.0f, -1.0f, 0.0f,   //1          
			x, 0.0f, -z,      1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   //2
			-x, 0.0f, z,      1.0f, 1.0f, 1.0f,         0.0f, 0.0f,   0.0f, -1.0f, 0.0f,   //0
			x, 0.0f, -z,      1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   0.0f, -1.0f, 0.0f,   //2
			-x, 0.0f, -z,     1.0f, 1.0f, 1.0f,         1.0f, 1.0f,   0.0f, -1.0f, 0.0f,   //3

			x, 0.0f, -z,      1.0f, 1.0f, 1.0f,         0.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   //2 //Left
			-x, 0.0f, -z,     1.0f, 1.0f, 1.0f,         1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   //3
			x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  -1.0f, 0.0f, 0.0f,   //6
			-x, 0.0f, -z,     1.0f, 1.0f, 1.0f,         1.0f, 1.0f,   -1.0f, 0.0f, 0.0f,   //3
			x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 1.0f,	  -1.0f, 0.0f, 0.0f,   //6
			-x, y, -z,		  1.0f, 1.0f, 1.0f,  	    1.0f, 0.0f,   -1.0f, 0.0f, 0.0f,   //7

			-x, 0.0f, z,      1.0f, 1.0f, 1.0f,         0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   //0 //Right
			-x, y, z,	      1.0f, 1.0f, 1.0f, 		0.0f, 0.0f,	   1.0f, 0.0f, 0.0f,   //4
			x, y, z,          1.0f, 1.0f, 1.0f,  	    0.0f, 1.0f,	   1.0f, 0.0f, 0.0f,   //5
			-x, 0.0f, z,      1.0f, 1.0f, 1.0f,         0.0f, 0.0f,    1.0f, 0.0f, 0.0f,   //0
			x, 0.0f, z,       1.0f, 1.0f, 1.0f,         1.0f, 0.0f,    1.0f, 0.0f, 0.0f,   //1 
			x, y, z,          1.0f, 1.0f, 1.0f,  	    0.0f, 1.0f,	   1.0f, 0.0f, 0.0f,   //5

		};

		//GLuint indices[] = {

		//0,4,5, //Right
		//0,1,5,
		//1,2,5, //Front
		//2,5,6,
		//2,3,6, //Left
		//3,6,7,
		//0,3,4, //Back
		//3,4,7,
		//4,6,7, //Top
		//4,5,6,
		//0,1,2, //Bottom
		//0,2,3

		//};

		glEnable(GL_DEPTH_TEST);
		//Create the VAO 
		glGenVertexArrays(1, &VAO);
		//Binds the specific VAO
		glBindVertexArray(VAO);

		//Create VBO for inside VAO
		glGenBuffers(1, &VBO);
		/*glGenBuffers(1, &IBO);*/

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);

		//Create Element/indices Buffer Object
		/*glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);*/

		//Connect the vertices created to the VBO 

		//First takes location based on shader, then number
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), 0);
		glEnableVertexAttribArray(0);
		//color attribute
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		//texture attribute - set strides to 8 * sizeof to account for the texture coordinates
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)(8 * sizeof(float)));
		glEnableVertexAttribArray(3);

		glDrawArrays(GL_TRIANGLES, 0, 36);

		//UnBind the buffer
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}


	//Equation to calculate the normal vectors of complex shapes - needs more work (result is unsatisfactory)
	void calcAverageNormals(std::vector<GLfloat> &vertices, unsigned int verticeCount, unsigned int vLength, unsigned int normalOffset,
		unsigned int nextVertice)
	{
		for (size_t i = 0; i < verticeCount; i += nextVertice)
		{
			
			unsigned int in0 = i;
			unsigned int in1 = i + vLength;
			unsigned int in2 = in1 + vLength;
			
			glm::vec3 v1(vertices.at(in1) - vertices.at(in0), vertices.at(in1 + 1) - vertices.at(in0 + 1), vertices.at(in1 + 2) - vertices.at(in0 + 2));
			glm::vec3 v2(vertices.at(in2) - vertices.at(in0), vertices.at(in2 + 1) - vertices.at(in0 + 1), vertices.at(in2 + 2) - vertices.at(in0 + 2));
			glm::vec3 normal = glm::cross(v1, v2);
			normal = glm::normalize(normal);

			in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
			vertices.at(in0) += normal.x; vertices.at(in0 + 1) += normal.y; vertices.at(in0 + 2) += normal.z;
			vertices.at(in1) += normal.x; vertices.at(in1 + 1) += normal.y; vertices.at(in1 + 2) += normal.z;
			vertices.at(in2) += normal.x; vertices.at(in2 + 1) += normal.y; vertices.at(in2 + 2) += normal.z;
			
		}
		for (size_t i = 0; i < verticeCount / vLength; i++)
		{
			unsigned int nOffset = i * vLength + normalOffset;
			std::cout << vertices.at(nOffset) << vertices.at(nOffset + 1) << vertices.at(nOffset + 2) << std::endl;
			glm::vec3 vec(vertices.at(nOffset), vertices.at(nOffset + 1), vertices.at(nOffset + 2));
			vec = glm::normalize(vec);
			vertices.at(nOffset) = vec.x; vertices.at(nOffset + 1) = vec.y; vertices.at(nOffset + 2) = vec.z;
		}
	}
};
