#include "EarClipping.hpp"

#include <list>

void ProjectPoly(const vec3 n, const std::vector<Vertex> & vertices, const std::vector<GLushort> & indices, std::vector<Point> & points)
{
	points.resize(indices.size());

	// Really fast and dirty projection
	if (n[0] >= n[1] && n[0] >= n[2])
	{
		for (unsigned int i = 0; i < indices.size(); ++i)
		{
			const Vertex & v = vertices[indices[i]];
			points[i].x = v.position[1];
			points[i].y = v.position[2];
		}
	}
	else if (n[1] >= n[0] && n[1] >= n[2])
	{
		for (unsigned int i = 0; i < indices.size(); ++i)
		{
			const Vertex & v = vertices[indices[i]];
			points[i].x = v.position[0];
			points[i].y = v.position[2];
		}
	}
	else if (n[2] >= n[0] && n[2] >= n[1])
	{
		for (unsigned int i = 0; i < indices.size(); ++i)
		{
			const Vertex & v = vertices[indices[i]];
			points[i].x = v.position[0];
			points[i].y = v.position[1];
		}
	}
}

bool IsConvex(Point prev, Point curr, Point next)
{
	float area2 = 0;
	area2 += prev.x * (next.y - curr.y);
	area2 += curr.x * (prev.y - next.y);
	area2 += next.x * (curr.y - prev.y);
	return (area2 <= 0);
}

float dot(Point a, Point b)
{
	return a.x * b.x + a.y * b.y;
}

bool PointInTriangle(Point P, Point A, Point B, Point C)
{
	Point v0; v0.x = C.x - A.x; v0.y = C.y - A.y;
	Point v1; v1.x = B.x - A.x; v1.y = B.y - A.y;
	Point v2; v2.x = P.x - A.x; v2.y = P.y - A.y;

	float dot00 = dot(v0, v0);
	float dot01 = dot(v0, v1); 
	float dot02 = dot(v0, v2); 
	float dot11 = dot(v1, v1); 
	float dot12 = dot(v1, v2);

	float invDenom = 1 / (dot00 * dot11 - dot01 * dot01);
	float u = (dot11 * dot02 - dot01 * dot12) * invDenom;
	float v = (dot00 * dot12 - dot01 * dot02) * invDenom;

	return ((u >= 0) && (v >= 0) && (u + v < 1));
}

bool TriangulizePoly(const std::vector<Point> & points, std::vector<unsigned int> & triangles, bool flip)
{
	triangles.clear();

	std::list<unsigned int> remaining;

	for (unsigned int i = 0; i < points.size(); ++i)
		remaining.push_back(i);

	if (flip)
		remaining.reverse();

	std::list<unsigned int>::iterator it =  remaining.begin();
	
	// Search for ears
	while (it != remaining.end() && remaining.size() > 2)
	{
		std::list<unsigned int>::iterator itp = it;
		std::list<unsigned int>::iterator itn = it;
		if (it == remaining.begin()) itp = --remaining.end(); else --itp;
		if (it == --remaining.end()) itn = remaining.begin(); else ++itn;

		bool ear = true;

		// Check if ear tip is convex
		if (!IsConvex(points[*itp], points[*it], points[*itn]))
		{
			ear = false;
		}
		
		if (ear)
		{
			// Check if other point in triangle
			for (std::list<unsigned int>::iterator it2 = remaining.begin(); it2 != remaining.end(); ++it2)
			{
				if (*it2 == *itp ||*it2 == *it || *it2 == *itn)
					continue;
				
				if (PointInTriangle(points[*it2], points[*itp], points[*it], points[*itn]))
				{
					ear = false;
					break;
				}
			}
		}
			
		if (ear)
		{
			triangles.push_back(*itp);
			triangles.push_back(*it);
			triangles.push_back(*itn);

			remaining.erase(it);
			it =  remaining.begin();

		}
		else
			it++;
	}

	// Check if correct number of triangles
	if (triangles.size() / 3.0 != points.size() - 2)
	{
		return false;
	}
	else
		return true;
}
