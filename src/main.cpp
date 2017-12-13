// Local Headers
#include "helpers.hpp"
#include "types.hpp"
#include "utils.hpp"

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
	// Load GLFW and Create a Window
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	mWindow = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Tank2017", nullptr, nullptr);

	// Check for Valid Context
	if (mWindow == nullptr) {
		fprintf(stderr, "Failed to Create OpenGL Context");
		return EXIT_FAILURE;
	}

	// Make the window's context current
	glfwMakeContextCurrent(mWindow);

	// Load OpenGL Functions
	if (!gladLoadGL()) {
		printf("Glad can't load!\n");
		return EXIT_FAILURE;
	}

	printf("OpenGL %s\n", (const char*)glGetString(GL_VERSION));
	printf("GLSL %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

	return EXIT_SUCCESS;
}

void init_map()
{	
	// Read map
	map.read_map("res/map.txt");
	map.read_texture_mapping("res/map_texture_mapping.txt");

	// Initialize vertices and texture coordinates
	map.init_vert();
	map.init_texc();
}

int main(void)
{
	if (init_window() != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

	init_map();

	std::string unit_vert, unit_frag, unit_geom;
	load_shader_file("shaders/unit.vert", unit_vert);
	load_shader_file("shaders/unit.frag", unit_frag);
	load_shader_file("shaders/unit.geom", unit_geom);

	Program program;
	program.attach(GL_VERTEX_SHADER, unit_vert);
	program.attach(GL_FRAGMENT_SHADER, unit_frag);
	program.attach(GL_GEOMETRY_SHADER, unit_geom);
	program.init("outColor");
	program.bind();

	VertexArrayObject vao;
	vao.bind();

	VertexBufferObject vbo_vert;
	vbo_vert.update(map.vert, getArrayLength(map.vert), 2);
	program.bindVertexAttribArray("pos", vbo_vert, 2, 0);
	vbo_vert.bind();

	VertexBufferObject vbo_texc;
	vbo_texc.update(map.texc, getArrayLength(map.texc), 2);
	program.bindVertexAttribArray("texc", vbo_texc, 2, 0);
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
		glDrawArrays(GL_LINES, 0, getArrayLength(map.vert) / 2);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
