#include "utils.hpp"
#include <fstream>

void load_shader_file(std::string filename, std::string& shader_source)
{
	std::ifstream fd(filename);
	shader_source = std::string(std::istreambuf_iterator<char>(fd),
		(std::istreambuf_iterator<char>()));
}
