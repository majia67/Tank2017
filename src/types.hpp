#pragma once

#include <cstdio>
#include <cassert>
#include <string>
#include <vector>
#include <fstream>

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

struct Unit
{
	Unit_Type type;
	float pos[2];
};

std::vector<glm::mat2> texture_mapping;

struct Map
{
	float vert[MAP_ROWS * MAP_COLS * 6 * 3];
	float texc[MAP_ROWS * MAP_COLS * 6 * 2];
	Unit_Type unit_types[MAP_ROWS][MAP_COLS];

	Map() { init(); }
	
	void init()
	{
		const float BLOCK_WIDTH = 2.0f / BOARD_SIZE;

		// Initialize vertices
		for (int i = 0; i < MAP_ROWS; i++) {
			for (int j = 0; j < MAP_COLS; j++) {
				int st = (i * MAP_COLS + j) * 6 * 3;

				// set x coord
				vert[st] = -1.0f + j * BLOCK_WIDTH;
				vert[st + 3] = -1.0f + (j + 1) * BLOCK_WIDTH;
				vert[st + 6] = vert[st + 3];
				vert[st + 9] = vert[st + 3];
				vert[st + 12] = vert[st];
				vert[st + 15] = vert[st];

				// set y coord
				vert[st + 1] = 1.0f - i * BLOCK_WIDTH;
				vert[st + 4] = vert[st + 1];
				vert[st + 7] = 1.0f - (i + 1) * BLOCK_WIDTH;
				vert[st + 10] = vert[st + 7];
				vert[st + 13] = vert[st + 7];
				vert[st + 16] = vert[st + 1];

				// set z coord
				vert[st + 2] = 0.0f;
				vert[st + 5] = 0.0f;
				vert[st + 8] = 0.0f;
				vert[st + 11] = 0.0f;
				vert[st + 14] = 0.0f;
				vert[st + 17] = 0.0f;
			}
		}
	}

	void init_texc()
	{
		// Initialize texture coordinates
		for (int i = 0; i < MAP_ROWS; i++) {
			for (int j = 0; j < MAP_COLS; j++) {
				int st = (i * MAP_COLS + j) * 6 * 2;

				int unit_type = static_cast<int>(unit_types[i][j]);

				// set x coord
				texc[st] = texture_mapping[unit_type][0].x;
				texc[st + 2] = texture_mapping[unit_type][1].x;
				texc[st + 4] = texc[st + 2];
				texc[st + 6] = texc[st + 4];
				texc[st + 8] = texc[st];
				texc[st + 10] = texc[st];

				// set y coord
				texc[st + 1] = texture_mapping[unit_type][0].y;
				texc[st + 3] = texc[st + 1];
				texc[st + 5] = texture_mapping[unit_type][1].y;
				texc[st + 7] = texc[st + 5];
				texc[st + 9] = texc[st + 5];
				texc[st + 11] = texc[st + 1];
			}
		}
	}

	void read_map(std::string filename)
	{
		std::ifstream fin;
		fin.open(filename, std::ifstream::in);
		int r, c;
		fin >> r >> c;
		assert(r == MAP_ROWS && c == MAP_COLS);

		for (int i = 0; i < r; i++) {
			for (int j = 0; j < c; j++) {
				int type;
				fin >> type;
				unit_types[i][j] = static_cast<Unit_Type>(type);
			}
		}

		fin.close();
	}

	void read_texture_mapping(std::string filename)
	{
		std::ifstream fin;
		fin.open(filename, std::ifstream::in);
		int num;
		fin >> num;

		for (int i = 0; i < num; i++) {
			glm::mat2 uv;
			fin >> uv[0].x >> uv[0].y >> uv[1].x >> uv[1].y;
			texture_mapping.push_back(uv);
		}

		fin.close();
	}

	void print()
	{
		printf("The Map is defined as following (vertices.xyz, texCoords.uv):\n");
		for (int i = 0; i < MAP_ROWS; i++) {
			for (int j = 0; j < MAP_COLS; j++) {
				printf("Block %d, %d\n", i, j);
				int st_v = (i * MAP_COLS + j) * 6 * 3;
				int st_t = (i * MAP_COLS + j) * 6 * 2;
				for (int k = 0; k < 6; k++) {
					printf("%.2f\t%.2f\t%.2f\t%.2f\t%.2f\t\n",
						vert[st_v + k * 3],
						vert[st_v + k * 3 + 1],
						vert[st_v + k * 3 + 2],
						texc[st_t + k * 2],
						texc[st_t + k * 2 + 1]
					);
				}
			}
		}
	}
};
