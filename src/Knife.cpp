#include "Knife.hpp"

Knife::Knife(Camera& cam, unsigned int width, unsigned int height) : 
	_startX(0.0f), _startY(0.0f),
	_endX(0.0f), _endY(0.0f),
	_animation(0.0f), _cam(cam),
	_width(width), _height(height)
{
	GLfloat vertices[] = {
		-0.01f, 0.0f,  0.00f,
		1.01f, 0.0f,  0.00f,
		0.00f, 0.0f, -0.01f,
		0.00f, 0.0f,  0.01f,
		1.00f, 0.0f, -0.01f,
		1.00f, 0.0f,  0.01f
	};

	glGenVertexArrays(1, &_vao);
	glBindVertexArray(_vao);

	glGenBuffers(1, &_vbo);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	glBindVertexArray(0);

	mat4x4_identity(_transform);

}

Knife::~Knife()
{
	if (_vbo) glDeleteBuffers(1, &_vbo);
	if (_vao) glDeleteVertexArrays(1, &_vao);
}

void Knife::SetStart(float x, float y)
{
	_startX = x;
	_startY = y;
}

void Knife::SetEnd(float x, float y)
{
	_endX = x;
	_endY = y;

	vec3 p;
	_cam.GetPosition(p);

	vec3 a, b;
	_cam.CastRay((2.0f * _startX) / _width - 1.0f, 1.0f - (2.0f * _startY) / _height, a);
	_cam.CastRay((2.0f * _endX) / _width - 1.0f, 1.0f - (2.0f * _endY) / _height, b);

	vec3_add(a, a, p);
	vec3_add(b, b, p);

	vec3 fw;
	fw[0] = 1.0f;
	fw[1] = 0.0f;
	fw[2] = 0.0f;

	vec3 dir;
	vec3_sub(dir, b, a);
	vec3_norm(dir, dir);

	float cosTheta = vec3_mul_inner(fw, dir);

	vec3 axis;
	vec3_mul_cross(axis, fw, dir);
	vec3_norm(axis, axis);

	quat r;
	quat_rotate(r, acos(cosTheta), axis);
	quat_norm(r, r);

	mat4x4 R;
	mat4x4_from_quat(R, r);

	mat4x4_translate(_transform, a[0], a[1], a[2]);
	mat4x4_mul(_transform, _transform, R);

	vec3 d;
	vec3_sub(d, a, b);
	float s = vec3_len(d);
	mat4x4_scale_aniso(_transform, _transform, s, 1.0f, 1.0f);

	_animation = 0.0f;
}

float Knife::Cut(vec3 p, vec3 n)
{
	_cam.GetPosition(p);

	vec3 a, b;
	_cam.CastRay((2.0f * _startX) / _width - 1.0f, 1.0f - (2.0f * _startY) / _height, a);
	_cam.CastRay((2.0f * _endX) / _width - 1.0f, 1.0f - (2.0f * _endY) / _height, b);

	vec3_add(a, a, p);
	vec3_add(b, b, p);

	vec3 c, d;
	vec3_sub(c, b, a);
	vec3_sub(d, p, a);

	vec3_mul_cross(n, c, d);
	vec3_norm(n, n);

	return vec3_len(c);
}

void Knife::Render(GLint program)
{
	glBindVertexArray(_vao);
	glUniformMatrix4fv(glGetUniformLocation(program, "model"), 1, GL_FALSE, *_transform);
	glUniform1fv(glGetUniformLocation(program, "animation"), 1, &_animation);
	glDrawArrays(GL_LINES, 0, 6);
	glBindVertexArray(0);
}

void Knife::Animate(double dt)
{
	if (_animation < 1)
		_animation += 4.0f * dt;
}