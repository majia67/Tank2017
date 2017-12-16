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

bool is_home_hit = false;

template<typename T, int size>
int getArrayLength(T(&)[size]) { return size; }

bool on_tank_move(int i)
{
    if (map.has_reached_edge(battle.tank[i])) {
        return false;
    }

    float step = float(cur_time - prev_time) * TANK_MOVE_STEP;
    Tank dummy = battle.tank[i];
    dummy.move(step);

    // Collision check
    std::vector<Unit*> coll_units = coll_grid.check_collision(dummy);
    if (coll_units.size() > 0) {
        return false;
    }
    else {
        coll_grid.remove(battle.tank[i], false);
        battle.tank[i].move(step);
        coll_grid.put(battle.tank[i], false);
    }

    return true;
}

bool on_tank_move(int i, Direction direction)
{
    if (battle.tank[i].change_direction(direction) == false) {
        return on_tank_move(i);
    }

    return true;
}

void on_bullet_firing(int i)
{
    static double last_firing_time[TANK_NUM] = { 0.0f };

    // Each tank can only fire one bullet at a time, and can't fire too fast
    if (!battle.bullet[i].is_visible && cur_time - last_firing_time[i] > 0.5) {
        battle.bullet[i].init(battle.tank[i]);
        coll_grid.put(battle.bullet[i], false);

        last_firing_time[i] = cur_time;
    }
}

void handle_bullet_moving()
{
    for (int i = 0; i < TANK_NUM; i++) {
        if (battle.bullet[i].is_visible) {
            Bullet &bullet = battle.bullet[i];
            coll_grid.remove(bullet, false);

            if (map.has_reached_edge(bullet)) {
                bullet.is_visible = false;
                continue;
            }

            battle.bullet[i].move(float(cur_time - prev_time) * BULLET_MOVE_STEP);

            // Collision check
            std::vector<Unit*> coll_units = coll_grid.check_collision(bullet);
            if (coll_units.size() > 0) {
                for (Unit* unit : coll_units) {
                    switch (unit->type)
                    {
                    case Unit_Type::brick:
                    case Unit_Type::bullet:
                        unit->is_visible = false;
                        coll_grid.remove(*unit, true);
                        break;
                    case Unit_Type::tank_enemy:
                        if (bullet.owner_type == Unit_Type::tank_user) {
                            unit->is_visible = false;
                            coll_grid.remove(*unit, false);

                            battle.enemy_num -= 1;
                            battle.enemy_left -= 1;
                        }
                        break;
                    case Unit_Type::tank_user:
                        if (bullet.owner_type == Unit_Type::tank_enemy) {
                            // The user is hit; reinitialized to the original position
                            coll_grid.remove(*unit, false);
                            battle.tank[0].init(Unit_Type::tank_user, MAP_ROWS - 1, 3);
                        }
                        break;
                    case Unit_Type::home:
                        // The home is hit; game over
                        is_home_hit = true;
                        break;
                    }
                }
                bullet.is_visible = false;
                continue;
            }

            coll_grid.put(bullet, false);
        }
    }
}

void handle_enemy_tanks()
{
    for (int i = 1; i < TANK_NUM; i++) {
        Tank &tank = battle.tank[i];
        if (tank.is_visible) {
            // Switch a direction if can't move
            if (on_tank_move(i) == false) {
                int dir = static_cast<int>(tank.direction);
                dir = (dir + rand() % 4) % 4;
                tank.change_direction(static_cast<Direction>(dir));
            }

            // Fire a bullet
            if (rand() % 1024 < 768) {
                on_bullet_firing(i);
            }
        } else if (battle.enemy_num < TANK_ENEMY_NUM && battle.enemy_num < battle.enemy_left) {
            // Make a new enemy
            int pos = rand() % 3;
            switch (pos) {
            case 0: tank.init(Unit_Type::tank_enemy, 0, 0); break;
            case 1: tank.init(Unit_Type::tank_enemy, 0, MAP_COLS / 2); break;
            case 2: tank.init(Unit_Type::tank_enemy, 0, MAP_COLS - 1); break;
            }
            tank.change_direction(Direction::down);
            coll_grid.put(tank, true);
        }
    }
}

void handle_keyboard()
{
    // Handle user tank movement
    if (glfwGetKey(mWindow, GLFW_KEY_UP) == GLFW_PRESS) {
        on_tank_move(0, Direction::up);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_DOWN) == GLFW_PRESS) {
        on_tank_move(0, Direction::down);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_LEFT) == GLFW_PRESS) {
        on_tank_move(0, Direction::left);
    }
    if (glfwGetKey(mWindow, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        on_tank_move(0, Direction::right);
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
                map.block[i][j].type == Unit_Type::sea ||
                map.block[i][j].type == Unit_Type::home)
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
	while (!glfwWindowShouldClose(mWindow) && !is_home_hit) {
		// Background Fill Color
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

        cur_time = glfwGetTime();

        // Main game logic
        handle_keyboard();
        handle_enemy_tanks();
        handle_bullet_moving();

        prev_time = cur_time;

		// Draw the map
        map.refresh_data();
        vao_map.bind();
        vbo_map_vert.update(map.vert, getArrayLength(map.vert), 2);
		glDrawArrays(GL_LINES, 0, getArrayLength(map.vert) / 2);

        // Draw the tanks
        battle.refresh_data();
        vao_battle.bind();
        vbo_battle_vert.update(battle.vert, getArrayLength(battle.vert), 2);
        glDrawArrays(GL_LINES, 0, getArrayLength(battle.vert) / 2);

		// Flip Buffers and Draw
		glfwSwapBuffers(mWindow);
		glfwPollEvents();
	}

	glfwTerminate();
	return EXIT_SUCCESS;
}
