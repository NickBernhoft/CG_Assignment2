/*
this code was written for assignment01 of CAP5725 on Aug 26, 2026

This project is designed to run on macos 11.7.11
all resources for this project to be compiled on macos are included in the directory
such as the GLFW binary and header
*/

#include <stdio.h>
#include <stdlib.h>

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif
#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE // just incase it doesnt detect glad

#include <GLFW/glfw3.h>
#include "ngls.h"


// function prototypes
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

int main()
{
	int width = 640;
	int height = 480;

	float vertices[] = {
	    -0.5f, -0.5f, 0.0f,
	     0.5f, -0.5f, 0.0f,
	     0.0f,  0.5f, 0.0f
	};

	int vertices_size = sizeof(vertices) / sizeof(float);


	// init glfw and make the window
	if (!glfwInit())
	{ printf("GLFW Failed to init!\n"); }

	// version window hints before window creation
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); 
    #endif

	GLFWwindow* window = glfwCreateWindow(width, height, "Nicholas Bernhoft: Assignment 02", NULL, NULL);
    if (!window)
    { printf("Error creating GLFW window!\n"); glfwTerminate();}



    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback); // user defined function key_callback();

	gladLoadGL(); // set glad context
	



	/* generate the frameworks needed to render our triangle.
		we need: VertevBuffer->VertexArray VertexShader->FragmentShader->Program */

	// create the Program
	GLuint program = buildProgram3("./shaders/vertex_shader.vs", "./shaders/fragment_shader.fs"); // my function
	glUseProgram(program);

	// create and bind the vao and vbo
	GLuint vbo = buildVertexBuffer(vertices, vertices_size); // my function
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

	glEnableVertexAttribArray(glGetAttribLocation(program, "vPos"));
	glVertexAttribPointer(glGetAttribLocation(program, "vPos"), 3, GL_FLOAT, GL_FALSE, 0, 0);




	/* Main Loop */
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); // set wireframe rendering mode

    double frameTime = 1.0 / 60.0;
    double lastFrame = glfwGetTime();
	while (!glfwWindowShouldClose(window))
	{
		//simple spin wait for now. optionally sleep to reduce CPU usage
		while (glfwGetTime() < lastFrame + frameTime)
        {
            // spin wait
        }

		glfwPollEvents();

		// background color
		glClearColor(0.0,0.5,0.5, 1.0);
		glClear(GL_COLOR_BUFFER_BIT);

		// actual opengl code goes here
		glDrawArrays(GL_TRIANGLES, 0, vertices_size/3);
		
		glfwSwapBuffers(window);
		lastFrame = glfwGetTime();
	}

	glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}



// note that this function gets these parameters by default, so they are not customizable
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
	{
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
}
