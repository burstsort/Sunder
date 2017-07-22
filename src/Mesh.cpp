#include "Mesh.hpp"

#include "Geometry.hpp"
#include "EarClipping.hpp"

Mesh::Mesh()
{
	_vao = 0;
	_vbo = 0;
	_ebo = 0;

	_originalVertexCount = 0;
	_polycount = 0;
}

Mesh::~Mesh()
{
	Unload();
}

bool Mesh::Load(const std::string& filename)
{
	Unload();

	// Load mesh from OBJ file
	std::vector<GLfloat> vertices_unindexed;
	if (!LoadOBJ(filename + ".obj", vertices_unindexed))
		return false;

	// Index the vertices
	IndexVertices(vertices_unindexed, _vertices, _indices);
	_originalVertexCount = _vertices.size();
	_polycount = _indices.size() / 3;

	// Set up vertex array
	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	// Set up vertex array buffer
	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);

	// Set up element array buffer
	glGenBuffers(1, &_ebo);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLushort), &_indices[0], GL_STATIC_DRAW);

	// Position pointer
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

	// UV pointer
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));

	// Normal pointer
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void*)(5 * sizeof(GLfloat)));

	// Unbind
	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

void Mesh::Unload()
{
	if (_ebo) glDeleteBuffers(1, &_ebo);
	if (_vbo) glDeleteBuffers(1, &_vbo);
	if (_vao) glDeleteVertexArrays(1, &_vao);

	_vao = 0;
	_vbo = 0;
	_ebo = 0;

	_originalVertexCount = 0;
	_polycount = 0;
}

GLuint Mesh::GetVAO()
{
	return _vao;
}

GLuint Mesh::GetVBO()
{
	return _vbo;
}

GLuint Mesh::GetEBO()
{
	return _ebo;
}

unsigned int Mesh::GetPolycount()
{
	return _polycount;
}

std::vector<GLushort> const & Mesh::GetIndices()
{
	return _indices;
}

GLushort Mesh::AddVertex(Vertex vertex)
{
	_vertices.push_back(vertex);
	return _vertices.size() - 1;
}

void Mesh::Slice(std::vector<GLushort> & indices, vec3 p, vec3 n, std::vector<GLushort> & indicesA, std::vector<GLushort> & indicesB)
{
	indicesA.clear();
	indicesB.clear();

	std::vector<GLushort> lines;

	// Iterate through triangles
	for (unsigned int i = 0; i < indices.size(); i += 3)
	{
		GLushort ia, ib, ic, ix, iy;

		// Get triangle indices
		ia = indices[i];
		ib = indices[i+1];
		ic = indices[i+2];

		// Cut a triangle
		Vertex x, y;
		bool res = SliceTriangle(p, n, _vertices, ia, ib, ic, x, y);

		// If triangle missed
		if (!res)
		{
			// Add indices
			if (DistanceToPlane(p, n, _vertices[ic].position) > 0)
			{ indicesA.push_back(ia); indicesA.push_back(ib); indicesA.push_back(ic); }
			else
			{ indicesB.push_back(ia); indicesB.push_back(ib); indicesB.push_back(ic); }
		}

		// If triangle sliced
		if (res)
		{
			// Add new vertices
				GLushort ip = FindVertex(_vertices, x);
				GLushort iq = FindVertex(_vertices, y);

				if (ip < (unsigned short) -1) ix = ip; else ix = AddVertex(x);
				if (iq < (unsigned short) -1) iy = iq; else iy = AddVertex(y);

				lines.push_back(ix);
				lines.push_back(iy);

			// Add indices
				std::vector<GLushort> * pA;
				std::vector<GLushort> * pB;

				if (DistanceToPlane(p, n, _vertices[ic].position) > 0)
				{ pA = &indicesA; pB = &indicesB; }
				else
				{ pA = &indicesB; pB = &indicesA; }

				pA->push_back(ix); pA->push_back(iy); pA->push_back(ic);
				pB->push_back(ia); pB->push_back(ib); pB->push_back(ix);
				pB->push_back(ib); pB->push_back(iy); pB->push_back(ix);
		}
	}

	// Convert lines to polygons
	std::vector<std::vector<GLushort> > polygons;
	LinesToPolys(_vertices, lines, polygons);

	// Triangulize polygons
	for (unsigned int j = 0; j < polygons.size(); ++j)
	{	
		std::vector<Point> points;
		ProjectPoly(n, _vertices, polygons[j], points);

		std::vector<unsigned int> triangles;
		if (!TriangulizePoly(points, triangles))
			TriangulizePoly(points, triangles, true);

		AddGeometryFromPoly(n, polygons[j], triangles, _vertices, indicesA, indicesB);
	}

	// Update vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Mesh::Reset()
{
	_vertices.resize(_originalVertexCount);

	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, _vertices.size() * sizeof(Vertex), &_vertices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
