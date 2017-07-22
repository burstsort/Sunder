#include "Camera.hpp"

Camera::Camera()
{
	_fov = DEFAULT_FOV;
	_aspect = 16.0f / 9.0f;

	_orbit = true;

	_position[0] = 0.0f;
	_position[1] = 0.0f;
	_position[2] = 4.0f;

	_target[0] = 0.0f;
	_target[1] = 0.0f;
	_target[2] = 0.0f;

	_up[0] = 0.0f;
	_up[1] = 1.0f;
	_up[2] = 0.0f;
		
	_distance = 4.0f;
	_angleH = 0;
	_angleV = 0;

	mat4x4_identity(_view);
	mat4x4_identity(_proj);
}

Camera::~Camera()
{

}

void Camera::ToggleMode()
{
	if (!_orbit)
	{
		_distance = vec3_len(_position);

		vec3 fwd;
		vec3_norm(fwd, _position);

		_angleH = -atan2(-fwd[0], fwd[2]);
		_angleV = asin(fwd[1]);

		vec3_scale(_target, _target, 0.0f);
	}

	_orbit = !_orbit;
}

void Camera::ChangeTarget(vec3 target)
{
	vec3_scale(_target, target, 1.0f);
}

void Camera::MoveForward(float amount)
{
	if (_orbit)
	{
		if (_distance - amount < MAX_DIST)
		{
			if (_distance - amount > MIN_DIST)
				_distance -= amount;
			else
				_distance = MIN_DIST;
		}
		else
			_distance = MAX_DIST;
	}
	else
	{
		vec3 movement;
		vec3_sub(movement, _target, _position);
		vec3_norm(movement, movement);
		vec3_scale(movement, movement, amount);
		vec3_add(_position, _position, movement);
		vec3_add(_target, _target, movement);
	}
}

void Camera::MoveRight(float amount)
{
	if (!_orbit)
	{
		vec3 right, movement;
		vec3_sub(movement, _target, _position);
		vec3_norm(movement, movement);
		vec3_mul_cross(right, _up, movement);
		vec3_scale(movement, right, amount);
		vec3_add(_position, _position, movement);
	}
}

void Camera::PitchUp(float amount)
{
	if (!_orbit)
		amount = -amount;

	if (_angleV + amount < M_PI * 0.4999f)
	{
		if (_angleV + amount > -M_PI * 0.4999f)
			_angleV += amount;
		else
			_angleV = -M_PI * 0.4999f;
	}
	else
		_angleV = M_PI * 0.4999f;
}

void Camera::YawRight(float amount)
{
	if (!_orbit)
		amount = -amount;

	if (_angleH + amount < M_PI)
	{
		if (_angleH + amount > -M_PI)
			_angleH += amount;
		else
			_angleH += amount + 2.0f * M_PI;
	}
	else
		_angleH += amount - 2.0f * M_PI;
}

void Camera::Update()
{
	if (_orbit)
	{
		_position[0] = _target[0] + _distance * sin(_angleH) * cos(_angleV);
		_position[1] = _target[1] + _distance * sin(_angleV);
		_position[2] = _target[2] + _distance * cos(_angleH) * cos(_angleV);

		vec3 y;
		y[0] = 0.0f;
		y[1] = 1.0f;
		y[2] = 0.0f;

		vec3 fwd;
		vec3_sub(fwd, _target, _position);
		vec3_norm(fwd, fwd);

		vec3 right;
		vec3_mul_cross(right, y, fwd);
		vec3_norm(right, right);

		vec3_mul_cross(_up, right, fwd);
	}
	else
	{
		_target[0] = _position[0] - sin(_angleH) * cos(_angleV);
		_target[1] = _position[1] - sin(_angleV);
		_target[2] = _position[2] - cos(_angleH) * cos(_angleV);

		vec3 y;
		y[0] = 0.0f;
		y[1] = 1.0f;
		y[2] = 0.0f;

		vec3 fwd;
		vec3_sub(fwd, _target, _position);
		vec3_norm(fwd, fwd);

		vec3 right;
		vec3_mul_cross(right, y, fwd);
		vec3_norm(right, right);

		vec3_mul_cross(_up, right, fwd);

		if (vec3_len(_position) > MAX_DIST)
		{
			vec3_norm(_position, _position);
			vec3_scale(_position, _position, MAX_DIST);
		}
	}

	vec3 eye, xaxis, yaxis, zaxis;
	vec3_scale(eye, _position, -1.0f);

	vec3_add(zaxis, _target, eye);
	vec3_norm(zaxis, zaxis);

	vec3_mul_cross(xaxis, _up, zaxis);
	vec3_mul_cross(yaxis, xaxis, zaxis);

	_view[0][0] = xaxis[0];
	_view[0][1] = yaxis[0];
	_view[0][2] = zaxis[0];
	_view[0][3] = 0.0f;

	_view[1][0] = xaxis[1];
	_view[1][1] = yaxis[1];
	_view[1][2] = zaxis[1];
	_view[1][3] = 0.0f;

	_view[2][0] = xaxis[2];
	_view[2][1] = yaxis[2];
	_view[2][2] = zaxis[2];
	_view[2][3] = 0.0f;

	_view[3][0] = vec3_mul_inner(xaxis, eye);
	_view[3][1] = vec3_mul_inner(yaxis, eye);
	_view[3][2] = vec3_mul_inner(zaxis, eye);
	_view[3][3] = 1.0f;

	_proj[0][0] = 1.0f / (tan(_fov * 0.5f) * _aspect);
	_proj[0][1] = 0.0f;
	_proj[0][2] = 0.0f;
	_proj[0][3] = 0.0f;

	_proj[1][0] = 0.0f;
	_proj[1][1] = 1.0f / tan(_fov * 0.5f);
	_proj[1][2] = 0.0f;
	_proj[1][3] = 0.0f;

	_proj[2][0] = 0.0f;
	_proj[2][1] = 0.0f;
	_proj[2][2] = -(NEAR_CLIP + FAR_CLIP) / (NEAR_CLIP - FAR_CLIP);
	_proj[2][3] = 1.0f;

	_proj[3][0] = 0.0f;
	_proj[3][1] = 0.0f;
	_proj[3][2] = (2.0f * NEAR_CLIP + FAR_CLIP) / (NEAR_CLIP - FAR_CLIP);
	_proj[3][3] = 0.0f;
}

void Camera::GetViewMatrix(mat4x4 view)
{
	mat4x4_dup(view, _view);
}

void Camera::GetProjectionMatrix(mat4x4 proj)
{
	mat4x4_dup(proj, _proj);
}

void Camera::UpdateViewProjectionMatrices(GLint program)
{
	glUniformMatrix4fv(glGetUniformLocation(program, "view"), 1, GL_FALSE, *_view);
	glUniformMatrix4fv(glGetUniformLocation(program, "projection"), 1, GL_FALSE, *_proj);
}

void Camera::GetPosition(vec3 pos)
{
	vec3_scale(pos, _position, 1.0f);
}

void Camera::GetForward(vec3 fwd)
{
	vec3_sub(fwd, _target, _position);
	vec3_norm(fwd, fwd);
}

void Camera::GetUp(vec3 up)
{
	vec3_scale(up, _up, 1.0f);
}

void Camera::CastRay(float x, float y, vec3 dir)
{
	mat4x4 view_inv, proj_inv;
	mat4x4_invert(view_inv, _view);
	mat4x4_invert(proj_inv, _proj);

	vec4 a;

	a[0] = x * 1.0f;
	a[1] = y * 1.0f;
	a[2] = 1.0f;
	a[3] = 1.0f;

	vec4 a2;
	mat4x4_mul_vec4(a2, proj_inv, a);

	a2[2] = 1.0f;
	a2[3] = 0.0f;

	vec4 a3;
	mat4x4_mul_vec4(a3, view_inv, a2);

	dir[0] = a3[0];
	dir[1] = a3[1];
	dir[2] = a3[2];

	vec3_norm(dir, dir);
}