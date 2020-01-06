#version 330 core

in vec3 gouraudLighting;

out vec4 fragColor;

void main() {
	fragColor = vec4(gouraudLighting, 1.0);
}
