#version 330 core

struct Light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

uniform Light light;
uniform vec3 view;

out Out {
	vec3 gouraud;
};

void main() {

	vec3 position = (viewMatrix * modelMatrix * vec4(aPosition, 1.0)).xyz;
	vec3 normal = normalMatrix * aNormal;

	Out.gouraud += light.ambient;

	vec3 lightDir = normalize(light.position - position);
	vec3 viewDir = normalize(view - position);

	float diffuse = max(0.0, dot(normal, lightDir));
	Out.gouraud += light.diffuse * diffuse;

	float specular = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32);
	Out.gouraud += light.specular * specular;

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);
}
