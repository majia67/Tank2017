// Local Headers
#include "helpers.hpp"
#include "shaders.hpp"
#include "types.hpp"

// System Headers
#include <GLFW/glfw3.h>

// Standard Headers
#include <cstdio>
#include <cstdlib>
#include <iostream>

GLFWwindow* mWindow;
Map map;
Unit tank_user;
std::vector<Unit> tank_enemies;

template<typename T, int size>
int getArrayLength(T(&)[size]) { return size; }

int init_window()
{
	// Initialize the library
	if (!glfwInit())
		return -1;

	// Activate supersampling
	glfwWindowHint(GLFW_SAMPLES, 8);

	// Ensure that we get at least a 3.2 context
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	// On apple we have to load a core profile with forward compatibility
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create a windowed mode window and its OpenGL context
	mWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Hello World", NULL, NULL);
	if (!mWindow)
	{
		glfwTerminate();
		return -1;
	}

	// Make the window's context current
	glfwMakeContextCurrent(mWindow);

#ifndef __APPLE__
	glewExperimental = true;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		/* Problem: glewInit failed, something is seriously wrong. */
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}
	glGetError(); // pull and savely ignonre unhandled errors like GL_INVALID_ENUM
	fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

	int major, minor, rev;
	major = glfwGetWindowAttrib(mWindow, GLFW_CONTEXT_VERSION_MAJOR);
	minor = glfwGetWindowAttrib(mWindow, GLFW_CONTEXT_VERSION_MINOR);
	rev = glfwGetWindowAttrib(mWindow, GLFW_CONTEXT_REVISION);
	printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
	printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
	printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	return 0;
}

void init_map()
{	
	// Read map
	map.read_map("res/map.txt");
	map.read_texture_mapping("res/map_texture_mapping.txt");
	map.init_texc();
}

int main(void)
{
	int ret_value = init_window();
	if (ret_value != 0) {
		return EXIT_FAILURE;
	}

	init_map();

	Program program;
	program.init(vertexSource, fragmentSource, "outColor");
	program.bind();

	VertexArrayObject vao;
	vao.bind();

	VertexBufferObject vbo_vert;
	vbo_vert.update(map.vert, getArrayLength(map.vert), 3);
	program.bindVertexAttribArray("position", vbo_vert, 3, 0);
	vbo_vert.bind();

	VertexBufferObject vbo_texc;
	vbo_texc.update(map.texc, getArrayLength(map.texc), 2);
	program.bindVertexAttribArray("texcoord", vbo_texc, 2, 0);
	vbo_texc.bind();

	Texture texture_map;
	texture_map.load(GL_TEXTURE0, "res/map.png");
	glUniform1i(program.uniform("texMap"), 0);

	// Rendering Loop
	while (!glfwWindowShouldClose(mWindow)) {
		// Background Fill Color
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// Draw the map
		glDrawArrays(GL_TRIANGLES, 0, getArrayLength(map.vert) / 3);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
