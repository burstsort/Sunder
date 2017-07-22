#version 330 core

in float _animation;
in vec3 _pos;

out vec4 color;

void main()
{
	float t = clamp(2.0*(_animation - 0.5 * _pos.x), 0.0, 1.0);
	float opacity = 1.0 - t;
	vec3 c;
	c.r = 1.0;
	c.g = - 2.0 * ((2.0 * t) - 1.0);
	c.b = c.g;

	color = vec4(c, opacity);
}