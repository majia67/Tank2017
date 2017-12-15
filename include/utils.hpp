#pragma once

#include <string>
#include <vector>
#include <glm/glm.hpp>

void load_shader_file(std::string filename, std::string& shader_source);

void read_texture_mapping(std::string filename, std::vector<glm::mat2> &texture_mapping);
