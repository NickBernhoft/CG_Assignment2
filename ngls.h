/* NGLS is short for Nick's Graphics Library Shortcuts*/

/* This is a single header library meant to provide easier and more connscise use of OpenGL.
The function names comply with the standards used in glad and GLEW and so on. */

#ifndef NGLS_H
#define NGLS_H

#ifdef __APPLE__
    #define GL_SILENCE_DEPRECATION
#endif

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE // just incase it doesnt detect glad

#include <stdio.h>
#include <GLFW/glfw3.h>


#define LOG_SIZE 4098


/* 
	This contains all the info needed to render some vertices 
	note: this will always be kind of wasteful computationally
		as we have to bind all the needed objects every time 
		we render a single object
*/
typedef struct 
{
	float* verts;
	GLsizei vertCount; 

	GLuint pgm; // program
	GLuint vbo;	// vertex buffer object
	GLuint vao;	// vertex array object
	GLuint vs;	// vertex shader
	GLuint fs;	// fragment shader
} ngRenderable; // ng in the namesake of this header.

/* Function Prototypes */

// openGL shortcuts
GLuint buildVertexBuffer(const float* vertices, size_t size);
const GLuint buildVertexShader(const char* vertex_shader_text);
const GLuint buildFragmentShader(const char* fragment_shader_text);

// each of these is somewhat more abstract and automated as the number goes up
const GLuint buildProgram(GLuint vertex_shader, const GLuint fragment_shader);
const GLuint buildProgram2(const char* vertex_shader_text, const char* fragment_shader_text);

// helper functions
const char* loadFile(char* filename); // note: this function was generated using AI


/*
creates a filled vertex buffer in VRAM and then returns a handle to it.
TODO: add error handling
*/
GLuint buildVertexBuffer(const float vertices[], size_t size)
{
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * size, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return vertex_buffer;
}



/*
returns a single vertex shader handle
*/
const GLuint buildVertexShader(const char* vertex_shader_text)
{
    const GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_text, NULL);
    glCompileShader(vertex_shader);

    // error checking
    GLint success;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[LOG_SIZE];
        glGetShaderInfoLog(vertex_shader, LOG_SIZE, NULL, infoLog);
        printf("ERROR: Vertex Shader Compilation Failed!\n%s\n", infoLog);
    }

    return vertex_shader;
}



/*
returns a single fragment shader handle
*/
const GLuint buildFragmentShader(const char* fragment_shader_text)
{
    const GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_text, NULL);
    glCompileShader(fragment_shader);

    // error checking
    GLint success;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[LOG_SIZE];
        glGetShaderInfoLog(fragment_shader, LOG_SIZE, NULL, infoLog);
        printf("ERROR: Vertex Shader Compilation Failed!\n%s\n", infoLog);
    }

    return fragment_shader;
}


/* builds a Program from VS and FS handles */
const GLuint buildProgram(GLuint vertex_shader, const GLuint fragment_shader)
{
    const GLuint program = glCreateProgram();
    glAttachShader(program, vertex_shader);
    glAttachShader(program, fragment_shader);
    glLinkProgram(program);


    GLint success;
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success)
    {
    	char infoLog[LOG_SIZE];
	    glGetProgramInfoLog(program, LOG_SIZE, NULL, &infoLog[0]);
	    printf("ERROR: Shader Program Linking Failed!\n%s\n", infoLog);
    }

    return program;
}

/* builds a paogram straight from the text file */
const GLuint buildProgram2(const char* vertex_shader_text, const char* fragment_shader_text)
{
	GLuint vs = buildVertexShader(vertex_shader_text);
	GLuint fs = buildFragmentShader(fragment_shader_text);
	return buildProgram(vs, fs);
}
    

int initRenderable2D(float* verts, GLuint program)
{
	// we should be able to extract the shader info from the program
	// we should be able to generate the VBO and size from the 
}








/*
creates a string with the contents of a file
note: this function was generated using AI on account of the fact that
	its a failty trivial thing in C which we learned in undergrad.
	i thought this was the perfect time for automation
*/
const char* loadFile(char* filename) {
    FILE* fp = fopen(filename, "rb"); // Open file in binary read mode
    char* buffer = NULL;
    long filesize = 0;

    if (!fp) {
        fprintf(stderr, "Failed to open file: %s\n", filename);
        return NULL;
    }

    // Go to the end of the file to get the size
    if (fseek(fp, 0L, SEEK_END) == 0) {
        filesize = ftell(fp);
        if (filesize == -1) {
            perror("ftell error");
            fclose(fp);
            return NULL;
        }

        // Allocate memory for the buffer (+1 for null terminator)
        buffer = (char*)malloc(sizeof(char) * (filesize + 1));
        if (!buffer) {
            fprintf(stderr, "loadFile() Memory allocation failed\n");
            fclose(fp);
            return NULL;
        }

        // Go back to the start of the file
        rewind(fp);

        // Read the entire file into memory
        size_t newLen = fread(buffer, sizeof(char), (size_t)filesize, fp);
        if (ferror(fp) != 0) {
            fprintf(stderr, "loadFile() Error reading file\n");
            free(buffer);
            fclose(fp);
            return NULL;
        } else {
            buffer[newLen] = '\0'; // Ensure null-termination
        }
    }

    fclose(fp);
    return buffer; // Caller is responsible for freeing the memory
}




#endif
