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

#define FAST_OBJ_IMPLEMENTATION
#include <fast_obj.h>

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include "ngls.h"

typedef struct
{
	float 	x, y, z, 	// vertex xyz coords
			nx, ny, nz,	// normal vector xyz
			u, v; 		// texture coords
} obj_vertex;



/* function prototypes */

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

// creates an array of obj_vertex structs, whos order will be reflected in the shader program 
obj_vertex* create_geometry_buffer(const fastObjMesh* mesh, unsigned int* out_count);
obj_vertex* buildObjBuffer(const char* fileName, unsigned int *vertCount);

int main()
{
	int width = 640;
	int height = 480;
	unsigned int vertCount = 0;

	obj_vertex* vertices = buildObjBuffer("teapot.obj", &vertCount);
	

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
	GLuint vbo = buildVertexBuffer(vertices, vertCount); // my function
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);


    /* define the uniforms */
    // transformation
    GLint trans_matrix_location = glGetUniformLocation(program, "trans_matrix");
	mat4 tm;
    glm_mat4_identity(tm);

	vec3 scale_amount = { 0.05f, 0.05f, 0.05f };
    glm_scale(tm, scale_amount);

	glUniformMatrix4fv(trans_matrix_location, 1, GL_FALSE, (float*)tm);

    /* define the in Vecs */
    GLsizei stride = sizeof(obj_vertex);
    // in vec3 vpos;
	glEnableVertexAttribArray(glGetAttribLocation(program, "vpos"));
	glVertexAttribPointer(glGetAttribLocation(program, "vpos"), 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(obj_vertex, x));
	// in bec3 vnml;
	glEnableVertexAttribArray(glGetAttribLocation(program, "vnml"));
	glVertexAttribPointer(glGetAttribLocation(program, "vnml"), 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(obj_vertex, nx));
	// in vec2 vtex
	glEnableVertexAttribArray(glGetAttribLocation(program, "vtex"));
	glVertexAttribPointer(glGetAttribLocation(program, "vtex"), 2, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(obj_vertex, u));




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
		glDrawArrays(GL_TRIANGLES, 0, vertCount);
		
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


// creates an array of obj_vertex structs, whos order will be reflected in the shader program 
obj_vertex* create_geometry_buffer(const fastObjMesh* mesh, unsigned int* out_count)
{
    if (!mesh)
    { *out_count = 0; return NULL;}

    unsigned int total_indices = 0;
    for (unsigned int f = 0; f < mesh->face_count; ++f)
    {
        total_indices += mesh->face_vertices[f];
    }

    *out_count = total_indices;

    obj_vertex* buffer = malloc(total_indices * sizeof(obj_vertex));
    if (!buffer) return NULL;

    for (unsigned int i = 0; i < total_indices; ++i) {
        fastObjIndex idx = mesh->indices[i];

        buffer[i].x  = mesh->positions[3 * idx.p + 0];
        buffer[i].y  = mesh->positions[3 * idx.p + 1];
        buffer[i].z  = mesh->positions[3 * idx.p + 2];
        
        buffer[i].nx = mesh->normals[3 * idx.n + 0];
        buffer[i].ny = mesh->normals[3 * idx.n + 1];
        buffer[i].nz = mesh->normals[3 * idx.n + 2];
        
        buffer[i].u  = mesh->texcoords[2 * idx.t + 0];
        buffer[i].v  = mesh->texcoords[2 * idx.t + 1];
    }
    return buffer;
}


/* this function loads the obj file into the struct format we intend to use for our buffer
and writes the length of the array to an external variable */
obj_vertex* buildObjBuffer(const char* fileName, unsigned int *vertCount)
{
	// loading in the obj mesh
	fastObjMesh* mesh = fast_obj_read(fileName);
	if (!mesh) {
        printf("Failed to load .obj file.\n");
        return NULL;
    }

    printf("Mesh loaded successfully!\n");
    printf("faces: %u\n", mesh->face_count);
    printf("vertices: %u\n", mesh->position_count);

    obj_vertex* vertices = create_geometry_buffer(mesh, vertCount);

    printf("VertCount: %d\n", *vertCount);

    return vertices;
}