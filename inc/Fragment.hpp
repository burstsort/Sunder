#ifndef FRAGMENT
#define FRAGMENT

#include "linmath.h"

#include "Shader.hpp"
#include "Mesh.hpp"

#define FRICTION 10.0f

class Fragment
{
	public:
		Fragment(const std::vector<GLushort>& indices);
		~Fragment();

		void GetTransform(mat4x4 transform);
		void SetTransform(mat4x4 transform);

		void AddSpeed(vec3 speed);
		void GetSpeed(vec3 speed);

		unsigned int GetPolycount();
		std::vector<GLushort> & GetIndices();

		void UpdateIndices();
		void UpdatePhysics(double dt);

		void Render(GLint program);
		void RenderWireframe(GLint program);

	private:
		GLuint _ebo;

		std::vector<GLushort> _indices;
		unsigned int _polycount;

		vec3 _speed;
		mat4x4 _transform;
};

#endif //FRAGMENT