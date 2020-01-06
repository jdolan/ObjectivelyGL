#version 330 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aNormal;

uniform mat4 projectionMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;
uniform mat3 normalMatrix;

struct light {
	vec3 position;
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform light lights[8];

uniform vec3 camera;

out vec3 gouraudLighting;

void main() {

	gl_Position = projectionMatrix * viewMatrix * modelMatrix * vec4(aPosition, 1.0);

	vec3 position = (viewMatrix * modelMatrix * vec4(aPosition, 1.0)).xyz;
	vec3 normal = normalMatrix * aNormal;

	for (int i = 0; i < lights.length(); i++) {

		gouraudLighting += lights[i].ambient;

		vec3 lightDir = normalize(lights[i].position - position);
		vec3 viewDir = normalize(camera - position);

		float diffuse = max(0.0, dot(normal, lightDir));
		gouraudLighting += lights[i].diffuse * diffuse;

		float specular = pow(max(dot(viewDir, reflect(-lightDir, normal)), 0.0), 32);
		gouraudLighting += lights[i].specular * specular;
	}
}
