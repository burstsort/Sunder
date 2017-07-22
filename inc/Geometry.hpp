#ifndef GEOMETRY
#define GEOMETRY

#include <vector>
#include <linmath.h>

#include "Vertex.hpp"

void ProjectToPlane(Vertex p, vec3 n, float & u, float & v);
float DistanceToPlane(vec3 p, vec3 n, vec3 v);
bool IntersectLineSegmentWithPlane(vec3 p, vec3 n, vec3 a, vec3 b, float & d);
bool SliceTriangle(vec3 p, vec3 n, std::vector<Vertex> & vertices, GLushort & ia, GLushort & ib, GLushort & ic, Vertex & x, Vertex & y);

void LinesToPolys(const std::vector<Vertex>& vertices, std::vector<GLushort>& lines, std::vector<std::vector<GLushort> >& polygons);
void AddGeometryFromPoly(vec3 n, const std::vector<GLushort> & polygon, const std::vector<unsigned int> & triangles, std::vector<Vertex> & vertices, std::vector<GLushort> & indicesA, std::vector<GLushort> & indicesB);

#endif //GEOMETRY