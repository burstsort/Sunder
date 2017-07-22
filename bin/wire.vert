#version 330 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec2 _uv;
out vec3 _normal;

void main()
{
	_uv = uv;
	_normal = normalize(normal * transpose(mat3(model)));;

	gl_Position = projection * view * model * vec4(position, 1.0);
}