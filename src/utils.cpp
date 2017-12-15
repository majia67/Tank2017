#include "utils.hpp"
#include <fstream>

void load_shader_file(std::string filename, std::string& shader_source)
{
    std::ifstream fd(filename);
	shader_source = std::string(std::istreambuf_iterator<char>(fd),
		(std::istreambuf_iterator<char>()));
}

void read_texture_mapping(std::string filename, std::vector<glm::mat2> &texture_mapping)
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
