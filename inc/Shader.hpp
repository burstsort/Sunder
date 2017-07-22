#ifndef SHADER
#define SHADER

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>

class Shader
{
	public:
		Shader();
		~Shader();

		bool Load(const std::string& filename);
		void Unload();

		GLuint GetProgram();

	private:
		GLuint _program;
};

#endif //SHADER