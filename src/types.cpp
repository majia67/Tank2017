#include "types.hpp"
#include <cstdio>
#include <cassert>
#include <fstream>

std::vector<glm::mat2> texture_mapping;

void Map::init_vert()
{
	const float BLOCK_WIDTH = 2.0f / BOARD_SIZE;

	// Initialize vertices
	for (int i = 0; i < MAP_ROWS; i++) {
		for (int j = 0; j < MAP_COLS; j++) {
			int st = (i * MAP_COLS + j) * 2 * 2;
			// set the upper left corner
			vert[st] = -1.0f + j * BLOCK_WIDTH;
			vert[st + 1] = 1.0f - i * BLOCK_WIDTH;
			
			// set the lower right corner
			vert[st + 2] = -1.0f + (j + 1) * BLOCK_WIDTH;
			vert[st + 3] = 1.0f - (i + 1) * BLOCK_WIDTH;
		}
	}
}

void Map::init_texc()
{
	// Initialize texture coordinates
	for (int i = 0; i < MAP_ROWS; i++) {
		for (int j = 0; j < MAP_COLS; j++) {
			int st = (i * MAP_COLS + j) * 2 * 2;
			int unit_type = static_cast<int>(unit_types[i][j]);
			// set the upper left corner
			texc[st] = texture_mapping[unit_type][0].x;
			texc[st + 1] = texture_mapping[unit_type][0].y;

			// set the lower right corner
			texc[st + 2] = texture_mapping[unit_type][1].x;
			texc[st + 3] = texture_mapping[unit_type][1].y;
		}
	}
}

void Map::read_map(std::string filename)
{
	std::ifstream fin;
	fin.open(filename, std::ifstream::in);
	assert(fin.is_open());
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

void Map::read_texture_mapping(std::string filename)
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

void Map::print()
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
