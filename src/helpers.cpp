// Preprocessor Directives
#define STB_IMAGE_IMPLEMENTATION

// Local Headers
#include "helpers.hpp"

// System Headers
#include <cassert>
#include <iostream>
#include <fstream>

VertexArrayObject::VertexArrayObject()
{
	glGenVertexArrays(1, &id);
	check_gl_error();
}

void VertexArrayObject::bind()
{
	glBindVertexArray(id);
	check_gl_error();
}

void VertexArrayObject::free()
{
	glDeleteVertexArrays(1, &id);
	check_gl_error();
}

VertexBufferObject::VertexBufferObject()
{
	glGenBuffers(1, &id);
}

void VertexBufferObject::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	check_gl_error();
}

void VertexBufferObject::free()
{
	glDeleteBuffers(1, &id);
}

void VertexBufferObject::update(const GLfloat *M, int size, int attr_num)
{
	assert(id != 0);
	glBindBuffer(GL_ARRAY_BUFFER, id);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, M, GL_STATIC_DRAW);
	attrib_num = attr_num;
	check_gl_error();
}

void VertexBufferObject::update(const GLint *M, int size, int attr_num)
{
    assert(id != 0);
    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLint) * size, M, GL_STATIC_DRAW);
    attrib_num = attr_num;
    check_gl_error();
}

ElementBufferObject::ElementBufferObject()
{
	glGenBuffers(1, &id);
}

void ElementBufferObject::bind()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	check_gl_error();
}

void ElementBufferObject::update(const GLuint *M, int mRows, int mCols)
{
	assert(id != 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * mRows * mCols, M, GL_STATIC_DRAW);
	check_gl_error();
}

void ElementBufferObject::free()
{
	glDeleteBuffers(1, &id);
}

Program::Program()
{
	program_shader = glCreateProgram();
	vertex_shader = 0;
	fragment_shader = 0;
	geometry_shader = 0;
}

bool Program::init(const std::string &fragment_data_name)
{
	if (!vertex_shader || !fragment_shader)
		return false;

	glBindFragDataLocation(program_shader, 0, fragment_data_name.c_str());
	glLinkProgram(program_shader);

	GLint status;
	glGetProgramiv(program_shader, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		glGetProgramInfoLog(program_shader, 512, NULL, buffer);
		std::cerr << "Linker error: " << std::endl << buffer << std::endl;
		program_shader = 0;
		return false;
	}

	check_gl_error();
	return true;
}

void Program::attach(GLenum type, std::string &shader_string)
{
	GLuint shader = create_shader_helper(type, shader_string);
	glAttachShader(program_shader, shader);
	check_gl_error();

	switch (type)
	{
	case GL_VERTEX_SHADER: vertex_shader = shader; break;
	case GL_FRAGMENT_SHADER: fragment_shader = shader; break;
	case GL_GEOMETRY_SHADER: geometry_shader = shader; break;
	}
}

void Program::bind()
{
	glUseProgram(program_shader);
	check_gl_error();
}

GLint Program::attrib(const std::string &name) const
{
	return glGetAttribLocation(program_shader, name.c_str());
}

GLint Program::uniform(const std::string &name) const
{
	return glGetUniformLocation(program_shader, name.c_str());
}

GLint Program::bindVertexAttribArray(const std::string &name, VertexBufferObject& VBO) const
{
	GLint id = attrib(name);
	if (id < 0)
		return id;
	if (VBO.id == 0)
	{
		glDisableVertexAttribArray(id);
		return id;
	}
	VBO.bind();
	glEnableVertexAttribArray(id);
	glVertexAttribPointer(id, VBO.attrib_num, GL_FLOAT, GL_FALSE, 0, 0);
	check_gl_error();

	return id;
}

void Program::free()
{
	if (program_shader)
	{
		glDeleteProgram(program_shader);
		program_shader = 0;
	}
	if (vertex_shader)
	{
		glDeleteShader(vertex_shader);
		vertex_shader = 0;
	}
	if (fragment_shader)
	{
		glDeleteShader(fragment_shader);
		fragment_shader = 0;
	}
	if (geometry_shader)
	{
		glDeleteShader(geometry_shader);
		geometry_shader = 0;
	}
	check_gl_error();
}

GLuint Program::create_shader_helper(GLint type, const std::string &shader_string)
{
	if (shader_string.empty())
		return (GLuint)0;

	GLuint id = glCreateShader(type);
	const char *shader_string_const = shader_string.c_str();
	glShaderSource(id, 1, &shader_string_const, NULL);
	glCompileShader(id);

	GLint status;
	glGetShaderiv(id, GL_COMPILE_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		if (type == GL_VERTEX_SHADER)
			std::cerr << "Vertex shader:" << std::endl;
		else if (type == GL_FRAGMENT_SHADER)
			std::cerr << "Fragment shader:" << std::endl;
		else if (type == GL_GEOMETRY_SHADER)
			std::cerr << "Geometry shader:" << std::endl;
		std::cerr << shader_string << std::endl << std::endl;
		glGetShaderInfoLog(id, 512, NULL, buffer);
		std::cerr << "Error: " << std::endl << buffer << std::endl;
		return (GLuint)0;
	}
	check_gl_error();

	return id;
}

void Texture::load(GLenum active_texture, const std::string filename) {
	// Load the image; channel is currently not handled and is always set to RGB
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (!image) fprintf(stderr, "%s %s\n", "Failed to Load Texture", filename.c_str());

	GLenum format = GL_RGB;
	// Set the Correct Channel Format
	switch (channels)
	{
	case 1: format = GL_ALPHA;     break;
	case 2: format = GL_LUMINANCE; break;
	case 3: format = GL_RGB;       break;
	case 4: format = GL_RGBA;      break;
	}

	// Bind Texture and Set Filtering Levels
	glActiveTexture(active_texture);
	glBindTexture(GL_TEXTURE_2D, id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	// Release image pointer
	stbi_image_free(image);
}

void _check_gl_error(const char *file, int line)
{
	GLenum err(glGetError());

	while (err != GL_NO_ERROR)
	{
		std::string error;

		switch (err)
		{
		case GL_INVALID_OPERATION:      error = "INVALID_OPERATION";      break;
		case GL_INVALID_ENUM:           error = "INVALID_ENUM";           break;
		case GL_INVALID_VALUE:          error = "INVALID_VALUE";          break;
		case GL_OUT_OF_MEMORY:          error = "OUT_OF_MEMORY";          break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:  error = "INVALID_FRAMEBUFFER_OPERATION";  break;
		}

		std::cerr << "GL_" << error.c_str() << " - " << file << ":" << line << std::endl;
		err = glGetError();
	}
}
