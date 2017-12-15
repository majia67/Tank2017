#include "types.hpp"
#include <cstdio>
#include <cassert>
#include <fstream>
#include <utility>
#include <algorithm>

Unit::Unit()
{
    direction = Unit_Direction::up;
    id = unit_id_factory++;
    is_visible = false;
}

void Unit::init(Unit_Type unit_type, int row, int col)
{
    type = unit_type;
    is_visible = true;

    // set the upper left corner
    upleft.x = -1.0f + col * BLOCK_WIDTH;
    upleft.y = 1.0f - row * BLOCK_WIDTH;

    // set the lower right corner
    downright.x = -1.0f + (col + 1) * BLOCK_WIDTH;
    downright.y = 1.0f - (row + 1) * BLOCK_WIDTH;
}

bool Unit::change_direction(Unit_Direction direction)
{
    if (this->direction == direction) {
        return false;
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

    return true;
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
    if (r1.x <= l2.x || r2.x <= l1.x) {
        return false;
    }

    // if one unit is on the top of the other
    if (r1.y >= l2.y || r2.y >= l1.y) {
        return false;
    }

    return true;
}

void Tank::init(Unit_Type unit_type, int row, int col)
{
    Unit::init(unit_type, row, col);

    // set the upper left corner
    upleft.x += TANK_WIDTH_DELTA;
    upleft.y -= TANK_WIDTH_DELTA;

    // set the lower right corner
    downright.x -= TANK_WIDTH_DELTA;
    downright.y += TANK_WIDTH_DELTA;
}

void Tank::move(float step)
{
    switch (direction)
    {
    case Unit_Direction::up:
        upleft.y = std::min(1.0f, upleft.y + step);
        downright.y = upleft.y - TANK_WIDTH;
        break;
    case Unit_Direction::down:
        upleft.y = std::max(-1.0f, upleft.y - step);
        downright.y = upleft.y + TANK_WIDTH;
        break;
    case Unit_Direction::left:
        upleft.x = std::max(-1.0f, upleft.x - step);
        downright.x = upleft.x + TANK_WIDTH;
        break;
    case Unit_Direction::right:
        upleft.x = std::min(1.0f, upleft.x + step);
        downright.x = upleft.x - TANK_WIDTH;
        break;
    }
}

void Tanks::init()
{
    // Initialize the user tank
    tank[0].init(Unit_Type::tank_user, MAP_ROWS - 1, 3);

    // Initialize the enemy tank
    tank[1].init(Unit_Type::tank_enemy, 0, 0);
    tank[1].change_direction(Unit_Direction::down);
}

void Tanks::init_texc(std::vector<glm::mat2> &texture_mapping)
{
    for (int i = 0; i < TANK_NUM; i++) {
        int st = i * 2 * 2;
        int unit_type = static_cast<int>(tank[i].type);

        // set the upper left corner
        texc[st] = texture_mapping[unit_type][0].x;
        texc[st + 1] = texture_mapping[unit_type][0].y;

        // set the lower right corner
        texc[st + 2] = texture_mapping[unit_type][1].x;
        texc[st + 3] = texture_mapping[unit_type][1].y;
    }
}

void Tanks::refresh_data()
{
    for (int i = 0; i < TANK_NUM; i++)
    {
        int st = i * 2 * 2;
        vert[st] = tank[i].upleft.x;
        vert[st + 1] = tank[i].upleft.y;
        vert[st + 2] = tank[i].downright.x;
        vert[st + 3] = tank[i].downright.y;
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

void Map::init_texc(std::vector<glm::mat2> &texture_mapping)
{
	// Initialize texture coordinates
	for (int i = 0; i < MAP_ROWS; i++) {
		for (int j = 0; j < MAP_COLS; j++) {
			int st = (i * MAP_COLS + j) * 2 * 2;
			int unit_type = static_cast<int>(block[i][j].type);

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
            block[i][j].init(static_cast<Unit_Type>(type), i, j);
		}
	}

	fin.close();
}

void Map::refresh_data()
{
    // Initialize vertices
    for (int i = 0; i < MAP_ROWS; i++) {
        for (int j = 0; j < MAP_COLS; j++) {
            int st = (i * MAP_COLS + j) * 2 * 2;

            // set the upper left corner
            vert[st] = block[i][j].upleft.x;
            vert[st + 1] = block[i][j].upleft.y;

            // set the lower right corner
            vert[st + 2] = block[i][j].downright.x;
            vert[st + 3] = block[i][j].downright.y;
        }
    }
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
    int i = std::min(int((y - 1.0f) / -BLOCK_WIDTH), MAP_ROWS - 1);
    int j = std::min(int((x + 1.0f) / BLOCK_WIDTH), MAP_COLS - 1);
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

void Collision_Grid::put(Unit &unit)
{
#ifdef DEBUG
    printf("Unit %d type %d touches grid:", unit.id, unit.type);
    for (int grid_idx : get_grids_touched(unit)) {
        grid[grid_idx][unit.id] = unit;
        printf("\t%d", grid_idx);
    }
    printf("\n");
#else
    for (int grid_idx : get_grids_touched(unit)) {
        grid[grid_idx][unit.id] = unit;
    }
#endif // DEBUG
}

void Collision_Grid::put_by_center(Unit &unit)
{
    glm::vec2 center = (unit.upleft + unit.downright) / 2.0f;
    int grid_idx = get_grid_index(center.x, center.y);
    grid[grid_idx][unit.id] = unit;
}

void Collision_Grid::remove(Unit &unit)
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
        printf("Grid %d: ", i);
        for (std::pair<int, Unit> kv : grid[i]) {
            printf("Unit %d, Unit Type %d; \t", kv.first, kv.second.type);
        }
        printf("\n");
    }
}
