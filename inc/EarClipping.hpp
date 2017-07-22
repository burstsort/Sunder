#ifndef EAR_CLIPPING
#define EAR_CLIPPING

#include <vector>
#include <linmath.h>

#include "Geometry.hpp"

struct Point
{
    float x, y;
};

void ProjectPoly(const vec3 n, const std::vector<Vertex> & vertices, const std::vector<GLushort> & indices, std::vector<Point> & points);
bool TriangulizePoly(const std::vector<Point> & points, std::vector<unsigned int> & triangles, bool flip = false);

#endif //EAR_CLIPPING