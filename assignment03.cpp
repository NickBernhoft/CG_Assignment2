/*
this code was written for assignment03 of CAP5725 on october 25, 2026

This project is designed to run on windows
all resources for this project to be compiled on windows are included in the directory
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define GLAD_GL_IMPLEMENTATION
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE // just incase it doesnt detect glad

#include <cglm/cglm.h>
#include <GLFW/glfw3.h>
#include <atom_parser.cpp>
#include <ngls.h>



/* function prototypes */

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
GLFWwindow* buid_glfw_window(int width, int height);


int main(int argc, char *argv[])
{	
	int width = 640;
    int height = 640;
    GLFWwindow* window = buid_glfw_window(width, height);





	std::string pdbPath = "dna.pdb";
	BoundingVolume bounds{};
	std::vector<GPUAtom> atoms = parsePDBAndComputeBounds(pdbPath, bounds);

	// error handling & success print stats
	if (atoms.empty()) {
	    std::cerr << "No atoms were loaded." << std::endl;
	    return -1;
	}

	// print atom load stats
	std::cout << "Loaded " << atoms.size() << " atoms." << std::endl;
	std::cout << "Center: (" << bounds.centerX << ", " << bounds.centerY << ", " << bounds.centerZ << ")" << std::endl;
	std::cout << "Bounding Radius: " << bounds.boundingRadius << std::endl;




	/*building compute shader*/

	const char* compute_shader_text = loadFile("compute_shader.glsl");
	GLuint compute_program = buildComputeProgram(compute_shader_text);

	// window size for the output buffer
	glfwGetFramebufferSize(window, &width, &height);

	// create atom buffer which the compute shader will read
	GLuint atomSSBO;
	glGenBuffers(1, &atomSSBO);	
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, atomSSBO);
	glBufferData(GL_SHADER_STORAGE_BUFFER, atoms.size() * sizeof(GPUAtom), atoms.data(), GL_STATIC_DRAW);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, atomSSBO);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

	// build framebuffer
	GLuint outputTex;
	glGenTextures(1, &outputTex);
	glBindTexture(GL_TEXTURE_2D, outputTex);
	glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA32F, width, height);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);


	GLuint readFBO;
	glGenFramebuffers(1, &readFBO);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
	glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, outputTex, 0);
	glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);

	GLint atomCountLoc = glGetUniformLocation(compute_program, "uAtomCount");






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


		glUseProgram(compute_program);
		
		if (atomCountLoc != -1) {
            glUniform1i(atomCountLoc, static_cast<GLint>(atoms.size()));
        }

		// 2. Bind input SSBO (binding = 0) and output texture (binding = 0)
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, atomSSBO);
        glBindImageTexture(0, outputTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

		// 3. Dispatch 16x16 workgroups to cover the entire screen
        GLuint groupsX = (width + 15) / 16;
        GLuint groupsY = (height + 15) / 16;
        glDispatchCompute(groupsX, groupsY, 1);

        // 4. Wait until the compute shader finishes writing pixels
        glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

        // 5. Copy (blit) the output texture onto the default window framebuffer
        glBindFramebuffer(GL_READ_FRAMEBUFFER, readFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
        glBlitFramebuffer(0, 0, width, height,
                          0, 0, width, height,
                          GL_COLOR_BUFFER_BIT, GL_NEAREST);





		
		glfwSwapBuffers(window);
		lastFrame = glfwGetTime();
	}


	// cleanup
	glDeleteBuffers(1, &atomSSBO);
    glDeleteTextures(1, &outputTex);
    glDeleteFramebuffers(1, &readFBO);
    glDeleteProgram(compute_program);
    free((void*)compute_shader_text); // If loadFile() used malloc

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



GLFWwindow* buid_glfw_window(int width, int height)
{
	// init glfw and make the window
	if (!glfwInit())
	{ printf("GLFW Failed to init!\n"); }

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	


	GLFWwindow* window = glfwCreateWindow(width, height, "Nicholas Bernhoft: Assignment 03", NULL, NULL);
    if (!window)
    { printf("Error creating GLFW window!\n"); glfwTerminate();}

    glfwMakeContextCurrent(window);
	glfwSetKeyCallback(window, key_callback); // user defined function key_callback();

	gladLoadGL(); // set glad context
	return window;
}

