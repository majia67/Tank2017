#pragma once

#include "utils.hpp"
#include <string>
#include <vector>
#include <map>
#include <set>
#include <glm/glm.hpp>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BOARD_SIZE 11
#define MAP_ROWS BOARD_SIZE
#define MAP_COLS BOARD_SIZE
#define TANK_USER_NUM 1
#define TANK_ENEMY_NUM 5
#define TANK_NUM (TANK_USER_NUM + TANK_ENEMY_NUM)
#define TANK_WIDTH_DELTA 0.01f

const static float BLOCK_WIDTH = 2.0f / BOARD_SIZE;
const static float TANK_MOVE_STEP = BLOCK_WIDTH * 2;
const static float TANK_WIDTH = BLOCK_WIDTH - TANK_WIDTH_DELTA * 2;

enum class Unit_Type
{
	bg_gray = 0,
	bg_black = 1,
	road = 2,
	forest = 3,
	sea = 4,
	concrete = 5,
	brick = 6,
	home = 7,
	tank_enemy = 8,
	tank_user = 9,
};

enum class Unit_Direction
{
    up = 0,
    right = 1,
    down = 2,
    left = 3,
};

class Unit
{
public:
    int id;
	Unit_Type type;
    Unit_Direction direction;
    bool is_visible;
	glm::vec2 upleft;
	glm::vec2 downright;

    Unit();

    void init(Unit_Type unit_type, int row, int col);

    bool change_direction(Unit_Direction direction);

    bool is_overlap(Unit &unit);
};
static int unit_id_factory = 0;

class Tank : public Unit
{
public:

    void init(Unit_Type unit_type, int row, int col);

    void move(float step);
};

class Tanks
{
public:
    float vert[TANK_NUM * 2 * 2];
    float texc[TANK_NUM * 2 * 2];

    Tank tank[TANK_NUM];

    void init();

    void init_texc(std::vector<glm::mat2> &texture_mapping);

    void refresh_data();

    void print();
};

class Map
{
public:
	float vert[MAP_ROWS * MAP_COLS * 2 * 2];
	float texc[MAP_ROWS * MAP_COLS * 2 * 2];

    Unit block[MAP_ROWS][MAP_COLS];

	void init_texc(std::vector<glm::mat2> &texture_mapping);

	void read_map(std::string filename);

    void refresh_data();

	void print();
};

class Collision_Grid
{
public:
    std::map<int, Unit> grid[MAP_ROWS * MAP_COLS];

    int get_grid_index(float x, float y);
    std::set<int> get_grids_touched(Unit &unit);

    void put(Unit &unit);
    void put_by_center(Unit &unit);
    void remove(Unit &unit);

    std::vector<Unit> check_collision(Unit &unit);

    void print();
};
