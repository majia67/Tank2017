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
Tanks tanks;
Collision_Grid coll_grid;

VertexArrayObject vao_tank;
VertexBufferObject vbo_tank_vert;

template<typename T, int size>
int getArrayLength(T(&)[size]) { return size; }

void on_tank_move(int i, Unit_Direction direction, float step)
{
    if (tanks.tank[i].change_direction(direction) == false)
    {
        Tank dummy = tanks.tank[i];
        dummy.move(step);

        // Collision check
        std::vector<Unit> coll_units = coll_grid.check_collision(dummy);
        if (coll_units.size() > 0) {
            return;
        }
        else {
            coll_grid.remove(tanks.tank[i]);
            tanks.tank[i].move(step);
            coll_grid.put(tanks.tank[i]);
        }
    }
    tanks.refresh_data();

    vao_tank.bind();
    vbo_tank_vert.update(tanks.vert, getArrayLength(tanks.vert), 2);
}

void handle_keyboard(float delta_time)
{
    // Handle user tank movement
    if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::up, delta_time * TANK_MOVE_STEP);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::down, delta_time * TANK_MOVE_STEP);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::left, delta_time * TANK_MOVE_STEP);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::right, delta_time * TANK_MOVE_STEP);
    }
}

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

    glfwSetInputMode(mWindow, GLFW_STICKY_KEYS, 1);

	return EXIT_SUCCESS;
}

int main(void)
{
    if (init_window() != EXIT_SUCCESS) {
		return EXIT_FAILURE;
	}

    printf("OpenGL %s\n", (const char*)glGetString(GL_VERSION));
    printf("GLSL %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

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

    // Initialize texture
    std::vector<glm::mat2> texture_mapping;
    read_texture_mapping("res/map_texture_mapping.txt", texture_mapping);

    Texture texture_map;
    texture_map.load(GL_TEXTURE0, "res/map.png");
    glUniform1i(program.uniform("texMap"), 0);

    // Setting map
    map.read_map("res/map.txt");
    map.init_texc(texture_mapping);
    map.refresh_data();

	VertexArrayObject vao_map;
    vao_map.init();
	vao_map.bind();

    VertexBufferObject vbo_map_vert;
    vbo_map_vert.init();
	vbo_map_vert.update(map.vert, getArrayLength(map.vert), 2);
    program.bindVertexAttribArray("pos", vbo_map_vert);

	VertexBufferObject vbo_map_texc;
    vbo_map_texc.init();
	vbo_map_texc.update(map.texc, getArrayLength(map.texc), 2);
    program.bindVertexAttribArray("texc", vbo_map_texc);

    // Setting tanks
    tanks.init();
    tanks.init_texc(texture_mapping);
    tanks.refresh_data();

    vao_tank.init();
    vao_tank.bind();

    vbo_tank_vert.init();
    vbo_tank_vert.update(tanks.vert, getArrayLength(tanks.vert), 2);
    program.bindVertexAttribArray("pos", vbo_tank_vert);

    VertexBufferObject vbo_tank_texc;
    vbo_tank_texc.init();
    vbo_tank_texc.update(tanks.texc, getArrayLength(tanks.texc), 2);
    program.bindVertexAttribArray("texc", vbo_tank_texc);

    // Setting collision grid
    // Map units
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (map.block[i][j].type == Unit_Type::brick ||
                map.block[i][j].type == Unit_Type::concrete ||
                map.block[i][j].type == Unit_Type::sea)
            {
                coll_grid.put_by_center(map.block[i][j]);
            }
        }
    }
    // Tanks
    for (int i = 0; i < TANK_NUM; i++) {
        if (tanks.tank[i].is_visible) {
            coll_grid.put_by_center(tanks.tank[i]);
        }
    }

    // Timer
    double prev_time = glfwGetTime();

	// Rendering Loop
	while (!glfwWindowShouldClose(mWindow)) {
		// Background Fill Color
		glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        double cur_time = glfwGetTime();
        float delta_time = float(cur_time - prev_time);
        handle_keyboard(delta_time);

        prev_time = cur_time;

		// Draw the map
        vao_map.bind();
		glDrawArrays(GL_LINES, 0, getArrayLength(map.vert) / 2);

        // Draw the tanks
        vao_tank.bind();
        glDrawArrays(GL_LINES, 0, getArrayLength(tanks.vert) / 2);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
