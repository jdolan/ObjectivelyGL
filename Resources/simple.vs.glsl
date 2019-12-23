#version 330 core

layout (location = 0) in vec3 position;

uniform Matrix {
	mat4 projection;
	mat4 view;
	mat4 model;
} matrix;


void main() {
	gl_Position = matrix.projection * matrix.view * matrix.model * vec4(position, 1.0);
}
