#version 330 core
in vec3 fragNorm;

out vec4 color;

void main()
{
	vec3 normal = normalize(fragNorm);
	color = vec4(normal * 0.5 + 0.5, 1);
}
