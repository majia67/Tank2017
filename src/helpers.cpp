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
}

VertexArrayObject::~VertexArrayObject()
{
	glDeleteVertexArrays(1, &id);
}

void VertexArrayObject::bind()
{
	glBindVertexArray(id);
	check_gl_error();
}

void VertexBufferObject::bind()
{
	glBindBuffer(GL_ARRAY_BUFFER, id);
	check_gl_error();
}

VertexBufferObject::VertexBufferObject()
{
	glGenBuffers(1, &id);
}

VertexBufferObject::~VertexBufferObject()
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

ElementBufferObject::ElementBufferObject()
{
	glGenBuffers(1, &id);
}

ElementBufferObject::~ElementBufferObject()
{
	glDeleteBuffers(1, &id);
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

bool Program::init(
	const std::string &vertex_shader_string,
	const std::string &fragment_shader_string,
	const std::string &fragment_data_name)
{
	using namespace std;
	vertex_shader = create_shader_helper(GL_VERTEX_SHADER, vertex_shader_string);
	fragment_shader = create_shader_helper(GL_FRAGMENT_SHADER, fragment_shader_string);

	if (!vertex_shader || !fragment_shader)
		return false;

	program_shader = glCreateProgram();

	glAttachShader(program_shader, vertex_shader);
	glAttachShader(program_shader, fragment_shader);

	glBindFragDataLocation(program_shader, 0, fragment_data_name.c_str());
	glLinkProgram(program_shader);

	GLint status;
	glGetProgramiv(program_shader, GL_LINK_STATUS, &status);

	if (status != GL_TRUE)
	{
		char buffer[512];
		glGetProgramInfoLog(program_shader, 512, NULL, buffer);
		cerr << "Linker error: " << endl << buffer << endl;
		program_shader = 0;
		return false;
	}

	check_gl_error();
	return true;
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

GLint Program::bindVertexAttribArray(
	const std::string &name, VertexBufferObject& VBO, GLint attrib_num, GLint offset) const
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
	glVertexAttribPointer(id, attrib_num, GL_FLOAT, GL_FALSE, VBO.attrib_num * sizeof(GLfloat), (void*)(offset * sizeof(GLfloat)));
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
	check_gl_error();
}

GLuint Program::create_shader_helper(GLint type, const std::string &shader_string)
{
	using namespace std;
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
			cerr << "Vertex shader:" << endl;
		else if (type == GL_FRAGMENT_SHADER)
			cerr << "Fragment shader:" << endl;
		else if (type == GL_GEOMETRY_SHADER)
			cerr << "Geometry shader:" << endl;
		cerr << shader_string << endl << endl;
		glGetShaderInfoLog(id, 512, NULL, buffer);
		cerr << "Error: " << endl << buffer << endl;
		return (GLuint)0;
	}
	check_gl_error();

	return id;
}

void Texture::load(GLenum active_texture, const std::string filename) {
	// Load the image; channel is currently not handled and is always set to RGB
	unsigned char* image = stbi_load(filename.c_str(), &width, &height, &channels, 0);
	if (!image) fprintf(stderr, "%s %s\n", "Failed to Load Texture", filename.c_str());

	GLenum format;
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
