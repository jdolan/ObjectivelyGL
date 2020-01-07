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
#include <SDL.h>

#include <ObjectivelyGL.h>

typedef struct {
	SDL_Window *window;
	SDL_GLContext *context;
	Program *program;

	Model *model;
	VertexArray *vertexArray;
	Buffer *elementsBuffer;
	vec3s angles;

	vec3s eye;
	vec3s forward;
	vec3s right;
	vec3s up;

} View;

typedef struct {
	vec3s position;
	vec3s ambient;
	vec3s diffuse;
	vec3s specular;
} Light;

typedef struct {
	vec3s position;
	vec3s normal;
} Vertex;

/**
 * @brief Command Consumer to initialize GL resources.
 */
static void initialize(ident data) {

	View *in = data;

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

	in->context = SDL_GL_CreateContext(in->window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);

	in->model = $((Model *) alloc(WavefrontModel), initWithResourceName, "teapot.obj");

	in->eye = glms_vec3_scale(glms_vec3_add(in->model->mins, in->model->maxs), .5f);
	in->eye.z = in->model->maxs.z * 1.5f;

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "gouraud.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "gouraud.fs.glsl")
	);

	in->program = $(alloc(Program), initWithDescriptor, &descriptor);
	FreeProgramDescriptor(&descriptor);

	$(in->program, use);

	$(in->program, setUniformForName, "lights[0].position", &(vec3s) { 0, 20, 0 });
	$(in->program, setUniformForName, "lights[0].ambient", &(vec3s) { 1, 0, 0 });
	$(in->program, setUniformForName, "lights[0].diffuse", &(vec3s) { 0, 1, 0 });
	$(in->program, setUniformForName, "lights[0].specular", &(vec3s) { 0, 0, 1 });

	const Attribute attributes[] = MakeAttributes(
		MakeVertexAttributeVec3f(TagPosition, 0, Vertex, position),
		MakeVertexAttributeVec3f(TagNormal, 1, Vertex, normal)
	);

	in->vertexArray = $(in->model, vertexArray, attributes);
	in->elementsBuffer = $(in->model, elementsBuffer);

	glEnable(GL_DEPTH_TEST);
}

/**
 * @brief Command Consumer to draw the Scene.
 */
static void drawScene(ident data) {

	View *in = data;

	$(in->program, use);

	int w, h;
	SDL_GetWindowSize(in->window, &w, &h);

	const float aspect = w / (float) h;

	const mat4s projection = glms_perspective(90.f, aspect, .1f, 100.f);
	const mat4s view = glms_lookat(in->eye, GLMS_VEC3_ZERO, GLMS_YUP);
	const mat4s model = glms_euler_xyz(in->angles);
	const mat3s normal = glms_mat4_pick3(glms_mat4_transpose(glms_mat4_inv(model)));

	$(in->program, setUniformForName, "projectionMatrix", &projection);
	$(in->program, setUniformForName, "viewMatrix", &view);
	$(in->program, setUniformForName, "modelMatrix", &model);
	$(in->program, setUniformForName, "normalMatrix", &normal);
	$(in->program, setUniformForName, "eye", &in->eye);

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);
	$(in->vertexArray, enableAttribute, 1);

	$(in->elementsBuffer, bind, GL_ELEMENT_ARRAY_BUFFER);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, (GLsizei) in->model->elements->count, GL_UNSIGNED_INT, 0);

	assert(glGetError() == GL_NO_ERROR);

	SDL_GL_SwapWindow(in->window);
}

/**
 * @brief Command Consumer to destroy all GL resources.
 */
static void destroy(ident data) {

	View *in = data;

	release(in->model);
	release(in->program);
	release(in->vertexArray);
	release(in->elementsBuffer);

	SDL_GL_DeleteContext(in->context);
}

/**
 * @brief Program entry point.
 */
int main(int argc, char *argv[]) {

	SDL_Init(SDL_INIT_VIDEO);

	View in = {
		.window = SDL_CreateWindow(
			__FILE__,
			SDL_WINDOWPOS_CENTERED,
			SDL_WINDOWPOS_CENTERED,
			1024,
			640,
			SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI /*| SDL_WINDOW_FULLSCREEN_DESKTOP*/
		)
	};

	CommandQueue *queue = $(alloc(CommandQueue), init);

	$(queue, enqueue, initialize, &in);
	$(queue, start);

	while (true) {

		$(queue, waitUntilEmpty);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_MOUSEMOTION) {
				if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					in.angles.x += glm_rad(event.motion.yrel);
					in.angles.y += glm_rad(event.motion.xrel);
				}
			} else if (event.type == SDL_MOUSEWHEEL) {
				in.eye.z -= event.wheel.y;
			} else if (event.type == SDL_QUIT) {
				break;
			}
		}

		if (event.type == SDL_QUIT) {
			$(queue, enqueue, destroy, &in);
			break;
		}

		$(queue, enqueue, drawScene, &in);
	}

	$(queue, stop);

	release(queue);

	SDL_DestroyWindow(in.window);

	SDL_Quit();

	return 0;
}

