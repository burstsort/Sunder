#include "Shader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

Shader::Shader()
{
	_program = 0;
}

Shader::~Shader()
{
	Unload();
}

bool Shader::Load(const std::string & filename)
{
	GLint status;
	std::stringstream buffer;
	std::string source;
	const char* chars;

	// Read vertex shader file
	std::ifstream vsf((filename + ".vert").c_str());
	buffer << vsf.rdbuf();
	source = buffer.str();
	chars = source.c_str();
	
	// Compile vertex shader
	GLuint vsid = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vsid, 1, &chars, NULL);
	glCompileShader(vsid);
	
	// Check for errors
	glGetShaderiv(vsid, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::cout << "VERTEX SHADER ERROR" << std::endl;
		char buffer[512];
		glGetShaderInfoLog(vsid, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}

	// Empty buffer
	buffer.str(std::string());

	// Read fragment shader file
	std::ifstream fsf((filename + ".frag").c_str());
	buffer << fsf.rdbuf();
	source = buffer.str();
	chars = source.c_str();

	// Compile fragment shader
	GLuint fsid = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fsid, 1, &chars, NULL);
	glCompileShader(fsid);

	// Check for errors
	glGetShaderiv(fsid, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE)
	{
		std::cout << "FRAGMENT SHADER ERROR" << std::endl;
		char buffer[512];
		glGetShaderInfoLog(fsid, 512, NULL, buffer);
		std::cout << buffer;
		return false;
	}

	// Link shader program
	_program = glCreateProgram();
	glAttachShader(_program, vsid);
	glAttachShader(_program, fsid);
	glLinkProgram(_program);
	glDeleteShader(vsid);
	glDeleteShader(fsid);

	return true;
}

void Shader::Unload()
{
	if (_program)
		glDeleteProgram(_program);

	_program = 0;
}

GLuint Shader::GetProgram()
{
	return _program;
}

