#include "Fragment.hpp"

Fragment::Fragment(const std::vector<GLushort>& indices)
{
	_indices = indices;
	mat4x4_identity(_transform);

	glGenBuffers(1, &_ebo);
	UpdateIndices();

	vec3_scale(_speed, _speed, 0.0f);
}

Fragment::~Fragment()
{
	glDeleteBuffers(1, &_ebo);
}

void Fragment::GetTransform(mat4x4 transform)
{
	mat4x4_dup(transform, _transform);
}

void Fragment::SetTransform(mat4x4 transform)
{
	mat4x4_dup(_transform, transform);
}

void Fragment::AddSpeed(vec3 speed)
{
	vec3_add(_speed, _speed, speed);
}

void Fragment::GetSpeed(vec3 speed)
{
	vec3_scale(speed, _speed, 1.0f);
}

unsigned int Fragment::GetPolycount()
{
	return _polycount;
}

std::vector<GLushort> & Fragment::GetIndices()
{
	return _indices;
}

void Fragment::UpdateIndices()
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indices.size() * sizeof(GLushort), &_indices[0], GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	_polycount = _indices.size() / 3.0f;
}

void Fragment::UpdatePhysics(double dt)
{
	mat4x4 delta;
	mat4x4_translate(delta, _speed[0] * dt, _speed[1] * dt, _speed[2] * dt);
	mat4x4_mul(_transform, _transform, delta);

	vec3 frict;
	vec3_scale(frict, _speed, -FRICTION * dt);
	vec3_add(_speed, _speed, frict);
}

void Fragment::Render(GLint program)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, *_transform);
	glDrawElements(GL_TRIANGLES, _polycount * 3, GL_UNSIGNED_SHORT, (void*) 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Fragment::RenderWireframe(GLint program)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _ebo);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, *_transform);
	for(unsigned int i = 0; i < _polycount * 3.0f; i += 3)
		glDrawElements(GL_LINE_LOOP, 3, GL_UNSIGNED_SHORT, (void*) (i * sizeof(GLushort)));
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}