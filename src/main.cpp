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
Battle battle;
Collision_Grid coll_grid;

VertexArrayObject vao_map;
VertexArrayObject vao_battle;
VertexBufferObject vbo_map_vert;
VertexBufferObject vbo_battle_vert;

double prev_time, cur_time;

template<typename T, int size>
int getArrayLength(T(&)[size]) { return size; }

void update_map_vbo()
{
    map.refresh_data();
    vao_map.bind();
    vbo_map_vert.update(map.vert, getArrayLength(map.vert), 2);
}

void update_battle_vbo()
{
    battle.refresh_data();
    vao_battle.bind();
    vbo_battle_vert.update(battle.vert, getArrayLength(battle.vert), 2);
}

void on_tank_move(int i, Unit_Direction direction)
{
    float step = float(cur_time - prev_time) * TANK_MOVE_STEP;
    if (battle.tank[i].change_direction(direction) == false)
    {
        Tank dummy = battle.tank[i];
        dummy.move(step);

        // Collision check
        std::vector<Unit*> coll_units = coll_grid.check_collision(dummy);
        if (coll_units.size() > 0) {
            return;
        }
        else {
            coll_grid.remove(battle.tank[i], false);
            battle.tank[i].move(step);
            coll_grid.put(battle.tank[i], false);
        }
    }

    update_battle_vbo();
}

void on_bullet_firing(int i)
{
    static double last_firing_time[TANK_NUM] = { 0.0f };

    // Each tank can only fire one bullet at a time, and can't fire too fast
    if (!battle.bullet[i].is_visible && cur_time - last_firing_time[i] > 0.5) {
        battle.bullet[i].init(battle.tank[0]);
        coll_grid.put(battle.bullet[i], false);

        update_map_vbo();
        update_battle_vbo();

        last_firing_time[i] = cur_time;
    }
}

void handle_bullet_moving()
{
    for (int i = 0; i < TANK_NUM; i++) {
        if (battle.bullet[i].is_visible) {
            coll_grid.remove(battle.bullet[i], false);

            if (map.is_on_edge(battle.bullet[i])) {
                battle.bullet[i].is_visible = false;
                continue;
            }

            battle.bullet[i].move(float(cur_time - prev_time) * BULLET_MOVE_STEP);

            // Collision check
            std::vector<Unit*> coll_units = coll_grid.check_collision(battle.bullet[i]);
            if (coll_units.size() > 0) {
                for (Unit* unit : coll_units) {
                    if (unit->type == Unit_Type::brick) {
                        unit->is_visible = false;
                        coll_grid.remove(*unit, true);
                    }
                }
                battle.bullet[i].is_visible = false;
                continue;
            }

            coll_grid.put(battle.bullet[i], false);
        }
    }

    update_map_vbo();
    update_battle_vbo();
}

void handle_keyboard()
{
    // Handle user tank movement
    if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::up);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::down);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::left);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        on_tank_move(0, Unit_Direction::right);
    }

    // Handle firing the bullet
    if (glfwGetKey(mWindow, GLFW_KEY_SPACE) == GLFW_PRESS) {
        on_bullet_firing(0);
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

    vao_map.init();
	vao_map.bind();

    vbo_map_vert.init();
	vbo_map_vert.update(map.vert, getArrayLength(map.vert), 2);
    program.bindVertexAttribArray("pos", vbo_map_vert);

	VertexBufferObject vbo_map_texc;
    vbo_map_texc.init();
	vbo_map_texc.update(map.texc, getArrayLength(map.texc), 2);
    program.bindVertexAttribArray("texc", vbo_map_texc);

    // Setting tanks
    battle.init();
    battle.init_texc(texture_mapping);
    battle.refresh_data();

    vao_battle.init();
    vao_battle.bind();

    vbo_battle_vert.init();
    vbo_battle_vert.update(battle.vert, getArrayLength(battle.vert), 2);
    program.bindVertexAttribArray("pos", vbo_battle_vert);

    VertexBufferObject vbo_battle_texc;
    vbo_battle_texc.init();
    vbo_battle_texc.update(battle.texc, getArrayLength(battle.texc), 2);
    program.bindVertexAttribArray("texc", vbo_battle_texc);

    // Setting collision grid
    // Map units
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            if (map.block[i][j].type == Unit_Type::brick ||
                map.block[i][j].type == Unit_Type::concrete ||
                map.block[i][j].type == Unit_Type::sea)
            {
                coll_grid.put(map.block[i][j], true);
            }
        }
    }
    // Tanks
    for (int i = 0; i < TANK_NUM; i++) {
        if (battle.tank[i].is_visible) {
            coll_grid.put(battle.tank[i], true);
        }
    }

    // Timer
    prev_time = glfwGetTime();

	// Rendering Loop
	while (!glfwWindowShouldClose(mWindow)) {
		// Background Fill Color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        cur_time = glfwGetTime();

        handle_keyboard();
        handle_bullet_moving();

        prev_time = cur_time;

		// Draw the map
        vao_map.bind();
		glDrawArrays(GL_LINES, 0, getArrayLength(map.vert) / 2);

        // Draw the tanks
        vao_battle.bind();
        glDrawArrays(GL_LINES, 0, getArrayLength(battle.vert) / 2);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
