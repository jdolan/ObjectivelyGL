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
#include <ObjectivelyGL.h>

typedef struct {
	Program *program;
	const Variable *projection;
	const Variable *view;
	const Variable *model;
} Simple;

typedef struct {
	SDL_Window *window;
	SDL_GLContext *context;
	Simple simple;
	Model *model;
	VertexArray *vertexArray;
	Buffer *elementsBuffer;
} CommandData;

typedef struct {
	vec3s position;
} Vertex;

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
 * @brief Command Consumer to initialize GL resources.
 */
static void initialize(ident data) {

	CommandData *in = data;

	in->context = createContext(in->window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	in->simple.program = $(alloc(Program), initWithDescriptor, &descriptor);
	assert(in->simple.program);

	FreeProgramDescriptor(&descriptor);

	in->simple.projection = $(in->simple.program, uniformForName, "projection");
	in->simple.view = $(in->simple.program, uniformForName, "view");
	in->simple.model = $(in->simple.program, uniformForName, "model");

	in->model =	$((Model *) alloc(WavefrontModel), initWithResourceName, "teapot.obj");
	assert(in->model);

	const Attribute attributes[] = MakeAttributes(
		MakeVertexAttributeVec3f(TagPosition, 0, Vertex, position)
	);

	in->vertexArray = $(in->model, vertexArray, attributes);
	assert(in->vertexArray);

	in->elementsBuffer = $(in->model, elementsBuffer);
	assert(in->elementsBuffer);
}

/**
 * @brief Command Consumer to render the teapot.
 */
static void drawScene(ident data) {

	CommandData *in = data;

	int w, h;
	SDL_GetWindowSize(in->window, &w, &h);

	const mat4s projection = glms_perspective(90, (float) w / (float) h, 0, 100);
	const mat4s view = glms_lookat((vec3s) { 0, 16, -16 }, (vec3s) { 0, 0, 0 }, GLMS_YUP);
	const mat4s model = glms_euler_xyz((vec3s) { -GLM_PI_2, 0, SDL_GetTicks() * 0.001 });

	$(in->simple.program, use);
	$(in->simple.program, setUniform, in->simple.projection, &projection);
	$(in->simple.program, setUniform, in->simple.view, &view);
	$(in->simple.program, setUniform, in->simple.model, &model);

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);

	$(in->elementsBuffer, bind, GL_ELEMENT_ARRAY_BUFFER);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	glDrawElements(GL_TRIANGLES, (GLsizei) in->model->elements->count, GL_UNSIGNED_INT, 0);

	assert(glGetError() == GL_NO_ERROR);
	SDL_GL_SwapWindow(in->window);
}

/**
 * @brief Command Consumer to destroy all GL resources.
 */
static void destroy(ident data) {

	CommandData *in = data;

	release(in->simple.program);
	release(in->model);
	release(in->elementsBuffer);
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

