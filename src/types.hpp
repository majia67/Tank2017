#pragma once

#include <cstdio>

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768
#define BOARD_SIZE 12
#define MAP_ROWS BOARD_SIZE
#define MAP_COLS (BOARD_SIZE - 2)

struct Map
{
	float vert[MAP_ROWS * MAP_COLS * 6 * 3];
	float texc[MAP_ROWS * MAP_COLS * 6 * 2];

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

		// Initialize texture coordinates
		for (int i = 0; i < MAP_ROWS; i++) {
			for (int j = 0; j < MAP_COLS; j++) {
				int st = (i * MAP_COLS + j) * 6 * 2;

				// set x coord
				texc[st] = 0.0f;
				texc[st + 2] = 1.0f;
				texc[st + 4] = texc[st + 2];
				texc[st + 6] = texc[st + 4];
				texc[st + 8] = texc[st];
				texc[st + 10] = texc[st];

				// set y coord
				texc[st + 1] = 0.0f;
				texc[st + 3] = texc[st + 1];
				texc[st + 5] = 1.0f;
				texc[st + 7] = texc[st + 5];
				texc[st + 9] = texc[st + 5];
				texc[st + 11] = texc[st + 1];
			}
		}
	}

	void print() {
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
