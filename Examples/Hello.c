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
#include <Objectively/Thread.h>
#include <ObjectivelyGL.h>

#include "Teapot.h"

typedef struct {
	SDL_Window *window;
	SDL_GLContext *context;
	Program *program;
	VertexArray *vertexArray;
	UniformBuffer *uniformBuffer;
} CommandData;

#define UNIFORM_BUFFER_MATRIX 0

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
 * @brief Creates the Program from the "simple" vertex and fragment shaders.
 */
static Program *createProgram(void) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	if (program == NULL) {
		for (ShaderDescriptor *shader = descriptor.shaders;
			 shader->type != GL_NONE;
			 shader++) {

			if (shader->status != GL_TRUE) {
				fprintf(stderr, "%s\n", shader->infoLog);
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
 * @brief Creates the UniformBuffer for the "simple" vertex and fragment shaders.
 */
static UniformBuffer *createUniformBuffer(void) {

	UniformBuffer *uniformBuffer = $(alloc(UniformBuffer), init);
	if (uniformBuffer == NULL) {
		fprintf(stderr, "Failed to create uniform buffer\n");
		exit(1);
	}

	return uniformBuffer;
}

/**
 * Creates the VertexArray from the Teapot vertext data.
 */
static VertexArray *createVertexArray(void) {

	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER, sizeof(teapot), teapot, GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), initWithData, &data);
	if (buffer == NULL) {
		fprintf(stderr, "Failed to create buffer\n");
		exit(1);
	}

	const Attribute attributes[] = MakeAttributes(
		MakeAttribute(0, 3, GL_FLOAT, GL_FALSE, 0, NULL)
	);

	VertexArray *array = $(alloc(VertexArray), initWithAttributes, buffer, attributes);
	if (array == NULL) {
		fprintf(stderr, "Failed to create vertex array\n");
		exit(1);
	}

	release(buffer);
	return array;
}

/**
 * @brief Command Consumer to initialize GL resources.
 */
static void initialize(ident data) {

	CommandData *in = data;

	in->context = createContext(in->window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	in->program = createProgram();
	in->uniformBuffer = createUniformBuffer();
	in->vertexArray = createVertexArray();

	const GLint block = $(in->program, uniformBlockLocation, "Matrix");
	$(in->program, uniformBlockBinding, block, UNIFORM_BUFFER_MATRIX);
	$(in->uniformBuffer, bind, UNIFORM_BUFFER_MATRIX);
}

/**
 * @brief Command Consumer to render the teapot.
 */
static void drawScene(ident data) {

	CommandData *in = data;

	int w, h;
	SDL_GetWindowSize(in->window, &w, &h);

	struct {
		mat4s projection;
		mat4s view;
		mat4s model;
	} Matrix;

	Matrix.projection = glms_perspective(90, (float) w / (float) h, 0, 100);
	Matrix.view = glms_lookat((vec3s) { 0, 2, -2 }, (vec3s) { 0, 0, 0 }, GLMS_YUP);
	Matrix.model = glms_euler_xyz((vec3s) { 0, SDL_GetTicks() * 0.001, -M_PI });

	$((Buffer *) in->uniformBuffer, writeData, &MakeUniformBufferData(sizeof(Matrix), &Matrix, GL_STREAM_DRAW));

	$(in->program, use);

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glDrawArrays(GL_TRIANGLES, 0, lengthof(teapot) / 3);

	SDL_GL_SwapWindow(in->window);
}

/**
 * @brief Command Consumer to destroy all GL resources.
 */
static void destroy(ident data) {

	CommandData *in = data;

	release(in->program);
	release(in->uniformBuffer);
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

	CommandQueue *queue = $(alloc(CommandQueue), initWithCapacity, 10);

	$(queue, enqueue, initialize, &in);

	Thread *thread = $(queue, start);

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

	$(thread, cancel);
	$(thread, join, NULL);

	release(thread);

	SDL_DestroyWindow(in.window);

	SDL_Quit();

	return 0;
}

