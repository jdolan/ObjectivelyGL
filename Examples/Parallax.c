/*
 * ObjectivelyGL: Object oriented OpenGL framework for GNU C.
 * Copyright (C) 2014 Jay Dolan <jay@jaydolan.com>
 *
 * This software is provided 'as-is', without any express or implied
 * warranty. In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 * claim that you wrote the original software. If you use this software
 * in a product, an acknowledgment in the product documentation would be
 * appreciated but is not required.
 *
 * 2. Altered source versions must be plainly marked as such, and must not be
 * misrepresented as being the original software.
 *
 * 3. This notice may not be removed or altered from any source distribution.
 */

#include <assert.h>
#include <cglm/struct.h>
#include <SDL.h>
#include <SDL_image.h>
#include <ObjectivelyGL.h>

typedef struct {
	mat4s projection;
	mat4s view;
	mat4s model;
	vec3s lightPos;
	vec3s viewPos;
} Uniforms;

typedef struct {
	vec3s position;
	vec3s normal;
	vec2s diffuse;
	vec3s tangent;
	vec3s bitangent;
} Vertex;

typedef struct {
	SDL_Window *window;
	SDL_GLContext *context;
	Program *program;
	Uniforms uniforms;
	VertexArray *vertexArray;
	Buffer *elementsBuffer;
} CommandData;

/**
 * @brief Creates the GL context.
 */
static SDL_GLContext *createContext(SDL_Window *window) {

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);

	SDL_GLContext *context = SDL_GL_CreateContext(window);
	if (context == NULL) {
		fprintf(stderr, "Failed to create GL context\n");
		exit(1);
	}

	return context;
}

/**
 * @brief Creates the Program from the parallax vertex and fragment shaders.
 */
static Program *createProgram(void) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "Shaders/parallax.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "Shaders/parallax.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	if (program == NULL) {
		for (ShaderDescriptor *shader = descriptor.shaders;
			 shader->type != GL_NONE;
			 shader++) {

			if (shader->status != GL_TRUE) {
				fprintf(stderr, "%s: %s\n", shader->resources[0], shader->infoLog);
			}
		}
		fprintf(stderr, "%s\n", descriptor.infoLog);
		FreeProgramDescriptor(&descriptor);
		exit(1);
	}

	FreeProgramDescriptor(&descriptor);
	return program;
}

/**
 * @brief Command Consumer to initialize GL resources.
 */
static void initialize(ident data) {

	CommandData *in = data;

	in->context = createContext(in->window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	in->program = createProgram();

	Model *model = $((Model *) alloc(WavefrontModel), initWithResourceName, "Models/teapot.obj");

	const Attribute attributes[] = MakeAttributes(
		MakeVertexAttributeVec3f(TagPosition, 0, Vertex, position),
		MakeVertexAttributeVec3f(TagNormal, 1, Vertex, normal),
		MakeVertexAttributeVec2f(TagDiffuse, 2, Vertex, diffuse),
		MakeVertexAttributeVec3f(TagTangent, 3, Vertex, tangent),
		MakeVertexAttributeVec3f(TagBitangent, 4, Vertex, bitangent)
	);

	in->vertexArray = $(model, vertexArray, attributes);
	in->elementsBuffer = $(model, elementsBuffer);
}

/**
 * @brief Command Consumer to render the teapot.
 */
static void drawScene(ident data) {

	CommandData *in = data;

	int w, h;
	SDL_GetWindowSize(in->window, &w, &h);

	const mat4s projection = glms_perspective(90, (float) w / (float) h, 0, 100);
	$(in->program, setUniformForName, "projection", &projection);

	const mat4s view = glms_lookat((vec3s) { 0, 2, -2 }, (vec3s) { 0, 0, 0 }, GLMS_YUP);
	$(in->program, setUniformForName, "view", &view);

	const mat4s model = glms_mat4_identity();
	$(in->program, setUniformForName, "model", &model);

	$(in->program, use);

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	//glDrawElements(GL_TRIANGLES, lengthof(in->elementsBuffer), GL_UNSIGNED_INT, in->elements);

	SDL_GL_SwapWindow(in->window);
}

/**
 * @brief Command Consumer to destroy all GL resources.
 */
static void destroy(ident data) {

	CommandData *in = data;

	release(in->program);
	release(in->vertexArray);

	SDL_GL_DeleteContext(in->context);
}

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	CommandData in = {
		.window = SDL_CreateWindow(__FILE__,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1024,
			768,
			SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI
		)
	};

	CommandQueue *queue = $(alloc(CommandQueue), init);

	$(queue, enqueue, initialize, &in);
	$(queue, start);

	while (true) {

		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				break;
			}
		}

		if (event.type == SDL_QUIT) {
			$(queue, enqueue, destroy, &in);
			break;
		}

		$(queue, enqueue, drawScene, &in);
		$(queue, waitUntilEmpty);
	}

	$(queue, stop);

	release(queue);

	SDL_DestroyWindow(in.window);

	SDL_Quit();

	return 0;
}


