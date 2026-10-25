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

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

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
const GLuint buildVertexBuffer(void* vertices, size_t element_size, size_t numVertices);
const GLuint buildVertexShader(const char* vertex_shader_text);
const GLuint buildFragmentShader(const char* fragment_shader_text);

const GLuint buildProgram(GLuint vertex_shader, const GLuint fragment_shader);
const GLuint buildProgram2(const char* vertex_shader_text, const char* fragment_shader_text);
const GLuint buildProgram3(const char* vs_file, const char* fs_file);

const GLuint loadTexture(char* filename);

// helper functions
const char* loadFile(const char* filename); // note: this function was generated using AI


/*
Parameters: 
void* vertices: pointer to the array of data in RAM. can be a struct.
size_t element_size: size in bytes of the type of data youre buffering.
size_t numVertices: number of elements in the array you would like to buffer

*/
const GLuint buildVertexBuffer(void* vertices, size_t element_size, size_t numVertices)
{
    GLuint vertex_buffer;
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, element_size * numVertices, vertices, GL_STATIC_DRAW);
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

/* builds a program directly from the shader file names */
const GLuint buildProgram3(const char* vs_file, const char* fs_file)
{
    return buildProgram2(loadFile(vs_file), loadFile(fs_file));
}
    

/* outputs GL_TEXTURE_2D */
const GLuint loadTexture(char* filename)
{
    /* load the texture */
    int width, height, nrChannels;
    // Flip the image vertically on load if your texture coordinates expect (0,0) at the bottom-left
    stbi_set_flip_vertically_on_load(1); 
    unsigned char *data = stbi_load(filename, &width, &height, &nrChannels, 0);

    if (data == NULL) {
        fprintf(stderr, "stb_image failed to load \"%s\": %s\n", filename, stbi_failure_reason());
        return 0;
    }

    /* build the texture */
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    // Set texture wrapping parameters (e.g., GL_REPEAT)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    // Set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    /* load onto the GPU */
    // dynamic format
    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;

    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    stbi_image_free(data);
    return texture;
}


/*
creates a string with the contents of a file
note: this function was generated using AI on account of the fact that
	its a failty trivial thing in C which we learned in undergrad.
	i thought this was the perfect time for automation
*/
const char* loadFile(const char* filename) {
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
