#ifndef VERTEX
#define VERTEX

#include <GL/gl3w.h>
#include <GLFW/glfw3.h>

#include <string>
#include <vector>

struct Vertex
{
	GLfloat position[3];
	GLfloat uv[2];
	GLfloat normal[3];

	bool operator<(const Vertex that) const;
	bool operator==(const Vertex& that) const;
	Vertex& operator=(const Vertex& that);

};

bool LoadOBJ(const std::string& filename, std::vector<GLfloat>& vertices);
void IndexVertices(const std::vector<GLfloat>& in_vertices, std::vector<Vertex>& out_vertices, std::vector<GLushort>& out_indices);

void PrintVertex(const Vertex v);
unsigned short FindVertex(const std::vector<Vertex>& vertices, const Vertex v);
bool HasSamePosition(const Vertex& a, const Vertex& b);

#endif //VERTEX