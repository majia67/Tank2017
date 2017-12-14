#include "types.hpp"
#include <cstdio>
#include <cassert>
#include <fstream>
#include <utility>
#include <algorithm>

std::vector<glm::mat2> texture_mapping;

Unit::Unit() : Unit(Unit_Type::bg_gray) { }

Unit::Unit(Unit_Type unit_type) : type(unit_type)
{
    id = unit_id_factory++;
    direction = Unit_Direction::up;
    is_visible = false;
    upleft = glm::vec2(0.0f, 0.0f);
    downright = glm::vec2(0.0f, 0.0f);
}

void Unit::change_direction(Unit_Direction direction)
{
    if (this->direction == direction) {
        return;
    }

    if (this->direction == Unit_Direction::up && direction == Unit_Direction::down ||
        this->direction == Unit_Direction::down && direction == Unit_Direction::up ||
        this->direction == Unit_Direction::left && direction == Unit_Direction::right ||
        this->direction == Unit_Direction::right && direction == Unit_Direction::left)
    {
        std::swap(this->upleft, this->downright);
    }
    else
        if (this->direction == Unit_Direction::up && direction == Unit_Direction::right ||
            this->direction == Unit_Direction::right && direction == Unit_Direction::up ||
            this->direction == Unit_Direction::down && direction == Unit_Direction::left ||
            this->direction == Unit_Direction::left && direction == Unit_Direction::down)
        {
            std::swap(this->upleft.x, this->downright.x);
        }
        else
            if (this->direction == Unit_Direction::up && direction == Unit_Direction::left ||
                this->direction == Unit_Direction::left && direction == Unit_Direction::up ||
                this->direction == Unit_Direction::down && direction == Unit_Direction::right ||
                this->direction == Unit_Direction::right && direction == Unit_Direction::down)
            {
                std::swap(this->upleft.y, this->downright.y);
            }

    this->direction = direction;
}

bool Unit::is_overlap(Unit &unit)
{
    // ref: http://www.geeksforgeeks.org/find-two-rectangles-overlap/

    glm::vec2 l1(std::min(this->upleft.x, this->downright.x),
        std::max(this->upleft.y, this->downright.y));
    glm::vec2 r1(std::max(this->upleft.x, this->downright.x),
        std::min(this->upleft.y, this->downright.y));

    glm::vec2 l2(std::min(unit.upleft.x, unit.downright.x),
        std::max(unit.upleft.y, unit.downright.y));
    glm::vec2 r2(std::max(unit.upleft.x, unit.downright.x),
        std::min(unit.upleft.y, unit.downright.y));

    // if one unit is on the left side of the other
    if (r1.x < l2.x || r2.x < l1.x) {
        return false;
    }

    // if one unit is on the top of the other
    if (r1.y > l2.y || r2.y > l1.y) {
        return false;
    }

    return true;
}

void Tanks::init()
{
    // Initialize the user tank
    tanks[0] = Unit(Unit_Type::tank_user);
    init_tank(0, MAP_ROWS - 1, 3);

    // Initialize the enemy tank
    for (int i = 1; i < TANK_NUM; i++) {
        tanks[i] = Unit(Unit_Type::tank_enemy);
    }
    init_tank(1, 0, 0);
    change_direction(1, Unit_Direction::down);

    // Initialize texture coordinates (which does not change during the game)
    init_texc();
}

void Tanks::init_tank(int i, int row, int col)
{
    tanks[i].direction = Unit_Direction::up;
    tanks[i].is_visible = true;

    // set the upper left corner
    tanks[i].upleft.x = -1.0f + col * BLOCK_WIDTH;
    tanks[i].upleft.y = 1.0f - row * BLOCK_WIDTH;

    // set the lower right corner
    tanks[i].downright.x = -1.0f + (col + 1) * BLOCK_WIDTH;
    tanks[i].downright.y = 1.0f - (row + 1) * BLOCK_WIDTH;
}

void Tanks::init_texc()
{
    for (int i = 0; i < TANK_NUM; i++) {
        int st = i * 2 * 2;
        int unit_type = static_cast<int>(tanks[i].type);

        // set the upper left corner
        texc[st] = texture_mapping[unit_type][0].x;
        texc[st + 1] = texture_mapping[unit_type][0].y;

        // set the lower right corner
        texc[st + 2] = texture_mapping[unit_type][1].x;
        texc[st + 3] = texture_mapping[unit_type][1].y;
    }
}

void Tanks::change_direction(int i, Unit_Direction direction)
{
    tanks[i].change_direction(direction);
}

void Tanks::move(int i)
{
    switch (tanks[i].direction)
    {
    case Unit_Direction::up: 
        tanks[i].upleft.y = std::min(1.0f, tanks[i].upleft.y + TANK_MOVE_STEP);
        tanks[i].downright.y = tanks[i].upleft.y - BLOCK_WIDTH;
        break;
    case Unit_Direction::down:
        tanks[i].upleft.y = std::max(-1.0f, tanks[i].upleft.y - TANK_MOVE_STEP);
        tanks[i].downright.y = tanks[i].upleft.y + BLOCK_WIDTH;
        break;
    case Unit_Direction::left:
        tanks[i].upleft.x = std::max(-1.0f, tanks[i].upleft.x - TANK_MOVE_STEP);
        tanks[i].downright.x = tanks[i].upleft.x + BLOCK_WIDTH;
        break;
    case Unit_Direction::right:
        tanks[i].upleft.x = std::min(1.0f, tanks[i].upleft.x + TANK_MOVE_STEP);
        tanks[i].downright.x = tanks[i].upleft.x - BLOCK_WIDTH;
        break;
    }
}

void Tanks::refresh_data()
{
    for (int i = 0; i < TANK_NUM; i++)
    {
        int st = i * 2 * 2;
        vert[st] = tanks[i].upleft.x;
        vert[st + 1] = tanks[i].upleft.y;
        vert[st + 2] = tanks[i].downright.x;
        vert[st + 3] = tanks[i].downright.y;
    }
}

void Tanks::print()
{
    printf("The tanks are defined as following (vertices.xy, texCoords.uv):\n");
    for (int i = 0; i < TANK_NUM; i++) {
        int st_v = i * 2 * 2;
        int st_t = i * 2 * 2;
        for (int k = 0; k < 2; k++) {
            printf("%.2f\t%.2f\t%.2f\t%.2f\n",
                vert[st_v + k * 2],
                vert[st_v + k * 2 + 1],
                texc[st_t + k * 2],
                texc[st_t + k * 2 + 1]
            );
        }
    }
}

void Map::init()
{
    init_vert();
    init_texc();
}

void Map::init_vert()
{
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
	printf("The Map is defined as following (vertices.xy, texCoords.uv):\n");
	for (int i = 0; i < MAP_ROWS; i++) {
		for (int j = 0; j < MAP_COLS; j++) {
			printf("Block %d, %d\n", i, j);
			int st_v = (i * MAP_COLS + j) * 2 * 2;
			int st_t = (i * MAP_COLS + j) * 2 * 2;
			for (int k = 0; k < 2; k++) {
				printf("%.2f\t%.2f\t%.2f\t%.2f\n",
					vert[st_v + k * 2],
					vert[st_v + k * 2 + 1],
					texc[st_t + k * 2],
					texc[st_t + k * 2 + 1]
				);
			}
		}
	}
}

int Collision_Grid::get_grid_index(float x, float y)
{
    int i = std::min(int((x + 1.0f) / BLOCK_WIDTH), MAP_ROWS - 1);
    int j = std::min(int((y + 1.0f) / BLOCK_WIDTH), MAP_COLS - 1);
    return i * MAP_COLS + j;
}

std::set<int> Collision_Grid::get_grids_touched(Unit &unit)
{
    std::set<int> grids_touched;

    grids_touched.insert(get_grid_index(unit.upleft.x, unit.upleft.y));
    grids_touched.insert(get_grid_index(unit.upleft.x, unit.downright.y));
    grids_touched.insert(get_grid_index(unit.downright.x, unit.upleft.y));
    grids_touched.insert(get_grid_index(unit.downright.x, unit.downright.y));

    return grids_touched;
}

void Collision_Grid::put_into_grid(Unit &unit)
{
    for (int grid_idx : get_grids_touched(unit)){
        grid[grid_idx][unit.id] = unit;
    }
}

void Collision_Grid::remove_from_grid(Unit &unit)
{
    for (int grid_idx : get_grids_touched(unit)) {
        grid[grid_idx].erase(unit.id);
    }
}

std::vector<Unit> Collision_Grid::check_collision(Unit &unit)
{
    std::vector<Unit> unit_collides;

    for (int grid_idx : get_grids_touched(unit)) {
        for (std::pair<int, Unit> kv : grid[grid_idx]) {
            if (unit.id != kv.first && unit.is_overlap(kv.second)) {
                unit_collides.push_back(kv.second);
            }
        }
    }

    return unit_collides;
}

void Collision_Grid::print()
{
    for (int i = 0; i < MAP_ROWS * MAP_COLS; i++){
        printf("Grid %d:\n", i);
        for (std::pair<int, Unit> kv : grid[i]) {
            printf("Unit %d, Unit Type %d\n", kv.first, kv.second.type);
        }
    }
}
