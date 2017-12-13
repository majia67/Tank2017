#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BOARD_SIZE 11
#define MAP_ROWS BOARD_SIZE
#define MAP_COLS BOARD_SIZE

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

class Unit
{
public:
	Unit_Type type;
	float pos[2];
};

class Map
{
public:
	float vert[MAP_ROWS * MAP_COLS * 2 * 2];
	float texc[MAP_ROWS * MAP_COLS * 2 * 2];
	Unit_Type unit_types[MAP_ROWS][MAP_COLS];
	
	void init_vert();

	void init_texc();

	void read_map(std::string filename);

	void read_texture_mapping(std::string filename);

	void print();
};
