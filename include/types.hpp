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
#define TANK_ENEMY_NUM 3
#define TANK_ENEMY_MAX_NUM 10
#define TANK_NUM (TANK_USER_NUM + TANK_ENEMY_NUM)
#define TANK_WIDTH_DELTA 0.02f

const static float BLOCK_WIDTH = 2.0f / BOARD_SIZE;
const static float TANK_MOVE_STEP = BLOCK_WIDTH * 1.5;
const static float TANK_WIDTH = BLOCK_WIDTH - TANK_WIDTH_DELTA * 2;
const static float BULLET_WIDTH = BLOCK_WIDTH * 0.15f;
const static float BULLET_MOVE_STEP = BLOCK_WIDTH * 2.5f;

enum class Unit_Type
{
	bullet = 0,
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

enum class Direction
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
    Direction direction;
    bool is_visible;
	glm::vec2 upleft;
	glm::vec2 downright;

    Unit();

    void init(Unit_Type unit_type, int row, int col);

    bool change_direction(Direction direction);

    void move(float step);

    bool is_overlap(Unit &unit);
};
static int unit_id_factory = 0;

class Tank : public Unit
{
public:
    void init(Unit_Type unit_type, int row, int col);
};

class Bullet : public Unit
{
public:
    Unit_Type owner_type;

    void init(Tank tank);
};

class Battle
{
public:
    float vert[TANK_NUM * 2 * 6];
    float texc[TANK_NUM * 2 * 4];

    Tank tank[TANK_NUM];
    Bullet bullet[TANK_NUM];

    int enemy_num = 0;
    int enemy_left = TANK_ENEMY_MAX_NUM;

    void init();

    void init_texc(std::vector<glm::mat2> &texture_mapping);

    void refresh_data();

    void print();
};

class Map
{
public:
	float vert[MAP_ROWS * MAP_COLS * 2 * 6];
	float texc[MAP_ROWS * MAP_COLS * 2 * 2];

    Unit block[MAP_ROWS][MAP_COLS];

	void init_texc(std::vector<glm::mat2> &texture_mapping);

	void read_map(std::string filename);

    bool has_reached_edge(Unit &unit);

    void refresh_data();

	void print();
};

class Collision_Grid
{
public:
    std::map<int, Unit*> grid[MAP_ROWS * MAP_COLS];

    int get_grid_index(float x, float y);
    std::set<int> get_grids_touched(Unit &unit, bool by_center);

    void put(Unit &unit, bool by_center);
    void remove(Unit &unit, bool by_center);

    std::vector<Unit*> check_collision(Unit &unit);

    void print();
};
