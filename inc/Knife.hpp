#ifndef KNIFE
#define KNIFE

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include "linmath.h"

#include "Shader.hpp"
#include "Camera.hpp"

class Knife
{
	public:
		Knife(Camera& cam, unsigned int width, unsigned int height);
		~Knife();

		void SetStart(float x, float y);
		void SetEnd(float x, float y);
		float Cut(vec3 p, vec3 n);
		void Render(GLint program);

		void Animate(double dt);

	private:
		float _startX;
		float _startY;

		float _endX;
		float _endY;

		GLuint _vao;
		GLuint _vbo;
		mat4x4 _transform;

		float _animation;

		Camera& _cam;

		unsigned int _width;
		unsigned int _height;
};

#endif //KNIFE