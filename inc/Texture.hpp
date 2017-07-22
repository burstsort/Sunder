#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>


class Texture
{
	public:
		Texture();
		~Texture();
		
		bool Load(const std::string& filename);
		void Unload();

		GLuint GetId();

	private:
		GLuint _id;
};

#endif // TEXTURE_HPP