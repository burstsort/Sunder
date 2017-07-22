#include "Geometry.hpp"

#include <iostream>
#include <list>

void ProjectToPlane(Vertex p, vec3 n, float & u, float & v)
{
	// TEMP - REPLACE WITH SOMETHING BETTER
	if (n[0] >= n[1] && n[0] >= n[2])
	{
		u = p.position[1];
		v = p.position[2];
	}
	else if (n[1] >= n[0] && n[1] >= n[2])
	{
		u = p.position[0];
		v = p.position[2];
	}
	else if (n[2] >= n[0] && n[2] >= n[1])
	{
		u = p.position[0];
		v = p.position[1];
	}
}

float DistanceToPlane(vec3 p, vec3 n, vec3 v)
{
	float upper = vec3_mul_inner(n, v)-vec3_mul_inner(n, p);
	float lower = sqrt(n[0]*n[0] + n[1]*n[1] + n[2]*n[2]);
	return upper/lower;
}

bool IntersectLineSegmentWithPlane(vec3 p, vec3 n, vec3 a, vec3 b, float & d)
{
	vec3 l, w;
	vec3_sub(l, b, a);
	vec3_sub(w, p, a);

	float div = vec3_mul_inner(l, n);
	if (div == 0.0f)
		d = 0.0f;
	else
		d = vec3_mul_inner(w, n) / div;

	return (d > 0.0f && d < 1.0f);
}

bool SliceTriangle(vec3 p, vec3 n, std::vector<Vertex> & vertices, GLushort & ia, GLushort & ib, GLushort & ic, Vertex & x, Vertex & y)
{
	Vertex a = vertices[ia];
	Vertex b = vertices[ib];
	Vertex c = vertices[ic];

	// Calculate distances between vertices and the plane
	float da, db, dc;
	da = DistanceToPlane(p, n, a.position);
	db = DistanceToPlane(p, n, b.position);
	dc = DistanceToPlane(p, n, c.position);
	
	// Check if the plane missed the triaangle
	if (da < 0 && db < 0 && dc < 0) return false;
	if (da > 0 && db > 0 && dc > 0) return false;

	// Intersect edges
	float dab, dbc, dca;
	bool ab = IntersectLineSegmentWithPlane(p, n, a.position, b.position, dab);
	bool bc = IntersectLineSegmentWithPlane(p, n, b.position, c.position, dbc);
	bool ca = IntersectLineSegmentWithPlane(p, n, c.position, a.position, dca);

	Vertex v, l, r;
	float dvl, drv;

	// Check which edges were sliced
	if (ab && !bc && ca)
	{
		v = a; l = b; r = c;
		dvl = dab; drv = dca;
		GLushort it = ia;
		ia = ib; ib = ic; ic = it;
	}
	else if (ab && bc && !ca)
	{
		v = b; l = c; r = a;
		dvl = dbc; drv = dab;
		GLushort it = ib;
		ib = ia; ia = ic; ic = it;
	}
	else if (!ab && bc && ca)
	{
		v = c; l = a; r = b;
		dvl = dca; drv = dbc;
	}
	else
	{
		std::cout << "SOMETHING WEIRD HAPPENED!!!" << std::endl;
		return false;
	}

	// Interpolate new vertices at intersection points
	vec3 t;
	vec3_sub(t, l.position, v.position);
	vec3_scale(x.position, t, dvl);
	vec3_add(x.position, x.position, v.position);

	vec3_sub(t, v.position, r.position);
	vec3_scale(y.position, t, drv);
	vec3_add(y.position, y.position, r.position);

	vec3_scale(x.normal, l.normal, dvl);
	vec3_scale(t, v.normal, (1.0f - dvl));
	vec3_add(x.normal, x.normal, t);
	vec3_norm(x.normal, x.normal);

	vec3_scale(y.normal, v.normal, drv);
	vec3_scale(t, r.normal, (1.0f - drv));
	vec3_add(y.normal, y.normal, t);
	vec3_norm(y.normal, y.normal);

	vec2 uv;
	vec2_scale(x.uv, l.uv, dvl);
	vec2_scale(uv, v.uv, (1.0f - dvl));
	vec2_add(x.uv, x.uv, uv);

	vec2_scale(y.uv, v.uv, drv);
	vec2_scale(uv, r.uv, (1.0f - drv));
	vec2_add(y.uv, y.uv, uv);

	return true;
}

void LinesToPolys(const std::vector<Vertex>& vertices, std::vector<GLushort>& lines, std::vector<std::vector<GLushort> >& polygons)
{
	polygons.clear();

	if (lines.size() < 2)
		return;

	while(!lines.empty())
	{
		// Take the first line
		GLushort first = lines[0];
		GLushort last = lines[1];

		std::list<GLushort> poly;
		poly.push_back(first);
		poly.push_back(last);

		lines.erase(lines.begin());
		lines.erase(lines.begin());

		// Find another lines it connects to
		for (unsigned int i = 0; i < lines.size(); i+=2)
		{
			if (HasSamePosition(vertices[last], vertices[lines[i]]))
			{
				poly.push_back(lines[i + 1]);
				last = lines[i + 1];
			}
			else if (HasSamePosition(vertices[last], vertices[lines[i + 1]]))
			{
				poly.push_back(lines[i]);
				last = lines[i];
			}
			else if (HasSamePosition(vertices[first], vertices[lines[i]]))
			{
				poly.push_front(lines[i + 1]);
				first = lines[i + 1];
			}
			else if (HasSamePosition(vertices[first], vertices[lines[i + 1]]))
			{
				poly.push_front(lines[i]);
				first = lines[i];
			}
			else
				continue;

			lines.erase(lines.begin() + i);
			lines.erase(lines.begin() + i);
			i = -2;
		}

		// Check if closed loop
		if (HasSamePosition(vertices[first], vertices[last]))
			polygons.push_back(std::vector<GLushort>(poly.begin(), --poly.end()));
		else
			std::cout << "open!" << std::endl;
	}
}

void AddGeometryFromPoly(vec3 n, const std::vector<GLushort> & polygon, const std::vector<unsigned int> & triangles, std::vector<Vertex>& vertices, std::vector<GLushort> & indicesA, std::vector<GLushort> & indicesB)
{
	float u, v;
	float au = 0;
	float av = 0;
	float maxu = 0;
	float minu = 999999;
	float maxv = 0;
	float minv = 999999;
	
	// Find max, min and average values
	for (unsigned int i = 0; i < polygon.size(); ++i)
	{
		ProjectToPlane(vertices[polygon[i]], n, u, v);
		au += u; av += v;

		if (u > maxu) maxu = u;
		if (u < minu) minu = u;
		if (v > maxv) maxv = v;
		if (v < minv) minv = v;
	}

	au /= polygon.size();
	av /= polygon.size();

	float su = (maxu - minu) * 2.0f;
	float sv = (maxv - minv) * 2.0f;

	GLushort startA = vertices.size();
	// Add one side
	for (unsigned int i = 0; i < polygon.size(); ++i)
	{
		Vertex & p = vertices[polygon[i]];
		Vertex v;
		
		v.position[0] = p.position[0];
		v.position[1] = p.position[1];
		v.position[2] = p.position[2];

		ProjectToPlane(p, n, v.uv[0], v.uv[1]);
		
		v.uv[0] = -0.5f - ((v.uv[0] - au) / su);
		v.uv[1] = -0.5f - ((v.uv[1] - av) / sv);

		v.normal[0] = -n[0];
		v.normal[1] = -n[1];
		v.normal[2] = -n[2];

		vertices.push_back(v);
	}

	GLushort startB = vertices.size();
	// Add the other side
	for (unsigned int i = 0; i < polygon.size(); ++i)
	{
		Vertex & p = vertices[polygon[i]];
		Vertex v;
		
		v.position[0] = p.position[0];
		v.position[1] = p.position[1];
		v.position[2] = p.position[2];

		ProjectToPlane(p, n, v.uv[0], v.uv[1]);

		v.uv[0] = -0.5f - ((v.uv[0] - au) / su);
		v.uv[1] = -0.5f - ((v.uv[1] - av) / sv);

		v.normal[0] = n[0];
		v.normal[1] = n[1];
		v.normal[2] = n[2];

		vertices.push_back(v);
	}

	// Add indices
	for (unsigned int i = 0; i < triangles.size() / 3; ++i)
	{
		vec3 norm;
		Vertex & a = vertices[polygon[triangles[3 * i]]];
		Vertex & b = vertices[polygon[triangles[3 * i + 1]]];
		Vertex & c = vertices[polygon[triangles[3 * i + 2]]];

		vec3 u, v;
		u[0] = b.position[0] - a.position[0];
		u[1] = b.position[1] - a.position[1];
		u[2] = b.position[2] - a.position[2];
		v[0] = c.position[0] - a.position[0];
		v[1] = c.position[1] - a.position[1];
		v[2] = c.position[2] - a.position[2];

		vec3_norm(u, u);
		vec3_norm(v, v);
		vec3_mul_cross(norm, u, v);
		vec3_norm(norm, norm);

		if (vec3_mul_inner(n, norm) < 0)
		{
			indicesA.push_back(startA + triangles[3 * i]);
			indicesA.push_back(startA + triangles[3 * i + 1]);
			indicesA.push_back(startA + triangles[3 * i + 2]);
			indicesB.push_back(startB + triangles[3 * i + 2]);
			indicesB.push_back(startB + triangles[3 * i + 1]);
			indicesB.push_back(startB + triangles[3 * i]);
		}
		else
		{
			indicesA.push_back(startA + triangles[3 * i + 2]);
			indicesA.push_back(startA + triangles[3 * i + 1]);
			indicesA.push_back(startA + triangles[3 * i]);
			indicesB.push_back(startB + triangles[3 * i]);
			indicesB.push_back(startB + triangles[3 * i + 1]);
			indicesB.push_back(startB + triangles[3 * i + 2]);
		}
	}
}

