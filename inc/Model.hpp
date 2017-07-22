#ifndef MODEL
#define MODEL

#include <vector>
#include "linmath.h"

#include "Mesh.hpp"
#include "Texture.hpp"
#include "Fragment.hpp"

class Model
{
	public:
		Model(Mesh* mesh, Texture* texOut, Texture* texIn);
		~Model();

		void ApplyTransform(mat4x4 transform);

		void UpdatePhysics(double dt);
		void Render(GLint program);
		void RenderWireframe(GLint program);

		void Freeze();
		void Unfreeze();
		bool IsFrozen();

		void Slice(vec3 point, vec3 normal, float separation = 1.0f);
		unsigned int Count();

		void Reset();

	private:
		Mesh* _mesh;
		Texture* _texOut;
		Texture* _texIn;

		bool frozen;

		std::vector<Fragment*> _fragments;

		mat4x4 _transform;
};

#endif //MODEL