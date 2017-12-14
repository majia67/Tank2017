#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <stb_image.h>
#include <glad/glad.h>

class VertexArrayObject
{
public:
	GLuint id;

	VertexArrayObject();

	// Select this VAO for subsequent draw calls
	void bind();

	void free();
};

class VertexBufferObject
{
public:
	GLuint id;
	int attrib_num;

	VertexBufferObject();

	// Updates the VBO with a 1D array M
	void update(const GLfloat *M, int size, int attr_num);
    void update(const GLint *M, int size, int attr_num);

	// Select this VBO for subsequent draw calls
	void bind();

	void free();
};

class ElementBufferObject
{
public:
	GLuint id;

	ElementBufferObject();

	// Updates the EBO with a 1D array M
	void update(const GLuint *M, int mRows, int mCols);

	// Select this EBO for subsequent draw calls
	void bind();

	void free();
};

// This class wraps an OpenGL program composed of two shaders
class Program
{
public:
	GLuint vertex_shader;
	GLuint fragment_shader;
	GLuint geometry_shader;
	GLuint program_shader;

	Program();

	// Create a new shader from the specified source strings
	bool init(const std::string &fragment_data_name);

	void attach(GLenum type, std::string &shader_string);

	// Select this shader for subsequent draw calls
	void bind();

	// Release all OpenGL objects
	void free();

	// Return the OpenGL handle of a named shader attribute (-1 if it does not exist)
	GLint attrib(const std::string &name) const;

	// Return the OpenGL handle of a uniform attribute (-1 if it does not exist)
	GLint uniform(const std::string &name) const;

	// Bind a per-vertex array attribute
	GLint bindVertexAttribArray(const std::string &name, VertexBufferObject& VBO) const;

	GLuint create_shader_helper(GLint type, const std::string &shader_string);

};

class Texture
{
public:
	GLuint id;
	int width;
	int height;
	int channels;

	Texture() { glGenTextures(1, &id); }
	~Texture() { glDeleteTextures(1, &id); }

	void load(GLenum active_texture, const std::string filename);
};

// From: https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)

#endif
