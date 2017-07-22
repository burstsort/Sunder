#version 330 core

in vec2 _uv;
in vec3 _normal;

uniform sampler2D texOut;
uniform sampler2D texIn;

out vec4 color;

void main()
{
	vec3 dir = normalize(vec3(2, 1, -3));

	vec3 shadow = vec3(0.57, 0.56, 0.51);
	vec3 light = vec3(0.93, 0.91, 0.84);

	float amount = clamp(dot(_normal, dir), 0.0, 1.0);

	color = vec4(shadow * (1 - amount) + light * amount, 1.0);
	
	if (_uv.x < 0 && _uv.y < 0)
		color = color * texture(texIn, _uv);
	else
		color = color * texture(texOut, _uv);

	color *= 1.5f;
	//color = vec4(_normal, 1.0);

	if (!gl_FrontFacing)
		color = vec4(0.8, 0.2, 0.2, 1.0); //color * vec4(1.0f, 0.5f, 0.5f, 1.0f);
}