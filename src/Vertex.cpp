#include "Vertex.hpp"

#include <cmath>
#include <limits>

#include <fstream>
#include <sstream>
#include <map>

#include <iostream>

#define EPSILON 10

bool Vertex::operator<(const Vertex that) const
{
	return memcmp((void*)this, (void*)&that, sizeof(Vertex)) > 0;
}

bool Vertex::operator==(const Vertex& that) const
{
	for (unsigned int i = 0; i < 3; ++i)
		if (!(std::fabs(this->position[i] - that.position[i]) < EPSILON * std::numeric_limits<float>::epsilon()))
			return false;
	
	for (unsigned int i = 0; i < 2; ++i)
		if (!(std::fabs(this->uv[i] - that.uv[i]) <  EPSILON * std::numeric_limits<float>::epsilon()))
			return false;

	for (unsigned int i = 0; i < 3; ++i)
		if (!(std::fabs(this->normal[i] - that.normal[i]) <  EPSILON * std::numeric_limits<float>::epsilon()))
			return false;
	
	return true;
}

Vertex& Vertex::operator=(const Vertex& that)
{
	memcpy((void*)this, (void*)&that, sizeof(Vertex));
	return *this;
}

bool LoadOBJ(const std::string& filename, std::vector<GLfloat>& vertices)
{
	vertices.clear();

	std::vector<GLfloat> v;
	std::vector<GLfloat> vt;
	std::vector<GLfloat> vn;

	std::vector<unsigned int> vi;
	std::vector<unsigned int> ti;
	std::vector<unsigned int> ni;

	std::ifstream file(filename.c_str());

	if (!file.good())
		return false;

	GLfloat a;
	std::string str;
	while (file.good())
	{
		file >> str;

		if (file.eof()) break;

		// Comment
		if (str[0] == '#')
		{
			getline(file, str);
		}
		// Position
		else if (str == "v")
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				file >> a;
				v.push_back(a);
			}
		}
		// UV
		else if (str == "vt")
		{
			for (unsigned int i = 0; i < 2; ++i)
			{
				file >> a;
				vt.push_back(a);
			}
		}
		// Normal
		else if (str == "vn")
		{
			for (unsigned int i = 0; i < 3; ++i)
			{
				file >> a;
				vn.push_back(a);
			}
		}
		// Triangle
		else if (str == "f")
		{
			unsigned int index;
			std::string vertex;

			for (unsigned int i = 0; i < 3; ++i)
			{
				file >> vertex;

				std::istringstream vs(vertex.substr(0, vertex.find('/')));
				vs >> index;
				vi.push_back(index - 1);

				vertex = vertex.substr(vertex.find('/') + 1);
				std::istringstream ts(vertex.substr(0, vertex.find('/')));
				ts >> index;
				ti.push_back(index - 1);

				vertex = vertex.substr(vertex.find('/') + 1);
				std::istringstream ns(vertex.substr(0, vertex.find('/')));
				ns >> index;
				ni.push_back(index - 1);
			}
		}
	}
	file.close();

	for (unsigned int i = 0; i < vi.size(); ++i)
	{
		vertices.push_back(v[vi[i] * 3]);
		vertices.push_back(v[vi[i] * 3 + 1]);
		vertices.push_back(v[vi[i] * 3 + 2]);
		vertices.push_back(vt[ti[i] * 2]);
		vertices.push_back(vt[ti[i] * 2 + 1]);
		vertices.push_back(vn[ni[i] * 3]);
		vertices.push_back(vn[ni[i] * 3 + 1]);
		vertices.push_back(vn[ni[i] * 3 + 2]);
	}

	return true;
}

void IndexVertices(const std::vector<GLfloat>& in_vertices, std::vector<Vertex>& out_vertices, std::vector<GLushort>& out_indices)
{
	out_vertices.clear();
	out_indices.clear();

	std::map<Vertex, unsigned short> binding;

	for (unsigned int i = 0; i < in_vertices.size() / 24.0f; ++i)
	{
		for (unsigned int j = 0; j < 3; ++j)
		{
			Vertex packed;
			packed.position[0] = in_vertices[i * 24 + j * 8];
			packed.position[1] = in_vertices[i * 24 + j * 8 + 1];
			packed.position[2] = in_vertices[i * 24 + j * 8 + 2];
			packed.uv[0] = in_vertices[i * 24 + j * 8 + 3];
			packed.uv[1] = in_vertices[i * 24 + j * 8 + 4];
			packed.normal[0] = in_vertices[i * 24 + j * 8 + 5];
			packed.normal[1] = in_vertices[i * 24 + j * 8 + 6];
			packed.normal[2] = in_vertices[i * 24 + j * 8 + 7];
			std::map<Vertex, unsigned short>::iterator it = binding.find(packed);
			if (it == binding.end())
			{
				out_vertices.push_back(packed);
				unsigned short index = (unsigned short) out_vertices.size() - 1;
				out_indices.push_back(index);
				binding[packed] = index;
			}
			else
				out_indices.push_back(it->second);
		}
	}
}

void PrintVertex(const Vertex v)
{
	std::cout << v.position[0] << " " << v.position[1] << " " << v.position[2] << std::endl;
	//std::cout << v.uv[0] << " " << v.uv[1] << std::endl;
	//std::cout << v.normal[0] << " " << v.normal[1] << " " << v.normal[2] << std::endl;
}

unsigned short FindVertex(const std::vector<Vertex>& vertices, const Vertex v)
{
	for (unsigned int i = 0; i < vertices.size() / 8; i++)
	{
		if (vertices[i] == v)
			return i;
	}

	return (unsigned short) -1;
}

bool HasSamePosition(const Vertex& a, const Vertex& b)
{
	for (unsigned int i = 0; i < 3; ++i)
		if (!(std::fabs(a.position[i] - b.position[i]) < EPSILON * std::numeric_limits<float>::epsilon()))
			return false;

	return true;
}