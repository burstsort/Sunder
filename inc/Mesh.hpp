#ifndef MESH
#define MESH

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

#include "linmath.h"

#include "Vertex.hpp"

class Mesh
{
	public:
		Mesh();
		~Mesh();

		bool Load(const std::string & filename);
		void Unload();

		GLuint GetVAO();
		GLuint GetVBO();
		GLuint GetEBO();

		unsigned int GetPolycount();
		std::vector<GLushort> const & GetIndices();

		GLushort AddVertex(Vertex vertex);

		void Slice(std::vector<GLushort> & indices, vec3 p, vec3 n, std::vector<GLushort> & indicesA, std::vector<GLushort> & indicesB);

		void Reset();

	private:
		GLuint _vao;
		GLuint _vbo;
		GLuint _ebo;

		std::vector<Vertex> _vertices;
		std::vector<GLushort> _indices;

		unsigned int _originalVertexCount;
		unsigned int _polycount;
};

#endif //MESH