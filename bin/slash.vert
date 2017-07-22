#version 330 core

layout (location = 0) in vec3 position;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float animation;

out float _animation;
out vec3 _pos;

void main()
{
	_animation = animation;
	_pos = position;
	gl_Position = projection * view * model * vec4(position, 1.0);
}