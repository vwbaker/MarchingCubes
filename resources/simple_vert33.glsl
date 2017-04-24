#version 330 core
layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNorm;
out vec3 fragNorm;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

void main()
{
	fragNorm = (P * V * M * vec4(normalize(vertNorm), 0)).xyz;
	gl_Position = P * V * M * vec4(vertPos, 1.0);
}
