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
	Camera *camera;
	Scene *scene;
	Model *model;
	Node *node;
	Program *program;
	VertexArray *vertexArray;
	Buffer *elementsBuffer;
	float frameTime;
	SDL_bool windowGrab;
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

typedef enum {
	TagMesh = 1 << 0
} SceneTag;

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

	in->camera = $(alloc(Camera), init);
	in->scene = $(alloc(Scene), init);

	in->model = $((Model *) alloc(WavefrontModel), initWithResourceName, "teapot.obj");

	in->camera->position = glms_vec3_scale(glms_vec3_add(in->model->mins, in->model->maxs), .5f);
	in->camera->position.z = in->model->maxs.z * 2.f;

	in->node = $(in->scene, addNode, NULL);
	in->node->tag = TagMesh;
	in->node->data = in->model;

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
 * @brief Draws a Node in the Scene.
 */
static void drawNode(const Node *node, ident data) {

	View *in = data;

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);
	$(in->vertexArray, enableAttribute, 1);

	$(in->elementsBuffer, bind, GL_ELEMENT_ARRAY_BUFFER);

	$(in->program, setUniformForName, "modelMatrix", &node->transform);

	const mat3s normal = glms_mat4_pick3(glms_mat4_transpose(glms_mat4_inv(node->transform)));
	$(in->program, setUniformForName, "normalMatrix", &normal);

	glDrawElements(GL_TRIANGLES, (GLsizei) in->model->elements->count, GL_UNSIGNED_INT, 0);

	assert(glGetError() == GL_NO_ERROR);
}

/**
 * @brief Command Consumer to draw the Scene.
 */
static void drawScene(ident data) {

	View *in = data;

	$(in->program, use);

	int width, height;
	SDL_GetWindowSize(in->window, &width, &height);

	const mat4s projection = $(in->camera, perspective, width, height);
	$(in->program, setUniformForName, "projectionMatrix", &projection);

	const mat4s view = $(in->camera, view);
	$(in->program, setUniformForName, "viewMatrix", &view);

	$(in->program, setUniformForName, "camera", &in->camera->position);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	$(in->scene, ready);
	$(in->scene, enumerateNodes, TagMesh, drawNode, in);

	SDL_GL_SwapWindow(in->window);
}

/**
 * @brief Command Consumer to destroy all GL resources.
 */
static void destroy(ident data) {

	View *in = data;

	release(in->camera);
	release(in->scene);
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

	unsigned int ticks = SDL_GetTicks();

	while (true) {

		$(queue, waitUntilEmpty);

		in.frameTime = (SDL_GetTicks() - ticks) / 1000.f;
		ticks = SDL_GetTicks();

		in.windowGrab = SDL_GetWindowGrab(in.window);

		SDL_Event event;
		while (SDL_PollEvent(&event)) {

			if (event.type == SDL_MOUSEBUTTONUP) {
				if (event.button.button == SDL_BUTTON_RIGHT) {
					if (in.windowGrab) {
						SDL_SetWindowGrab(in.window, false);
						SDL_SetRelativeMouseMode(false);
					} else {
						SDL_SetWindowGrab(in.window, true);
						SDL_SetRelativeMouseMode(true);
					}
				}
			}
			if (event.type == SDL_MOUSEMOTION) {
				if (SDL_GetWindowGrab(in.window)) {
					$(in.camera, freeLook, event.motion.xrel, event.motion.yrel);
				} else if (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
					in.node->angles.x += event.motion.yrel;
					in.node->angles.y += event.motion.xrel;
				}
			} else if (event.type == SDL_MOUSEWHEEL) {
				in.camera->fovY -= event.wheel.y;
			} else if (event.type == SDL_QUIT) {
				break;
			}
		}

		if (event.type == SDL_QUIT) {
			$(queue, enqueue, destroy, &in);
			break;
		}

		vec3s dir = glms_vec3_zero();
		const Uint8 *state = SDL_GetKeyboardState(NULL);
		if (state[SDL_SCANCODE_W]) {
			dir = glms_vec3_add(dir, in.camera->forward);
		}
		if (state[SDL_SCANCODE_S]) {
			dir = glms_vec3_sub(dir, in.camera->forward);
		}
		if (state[SDL_SCANCODE_A]) {
			dir = glms_vec3_sub(dir, in.camera->right);
		}
		if (state[SDL_SCANCODE_D]) {
			dir = glms_vec3_add(dir, in.camera->right);
		}
		if (state[SDL_SCANCODE_SPACE]) {
			dir = glms_vec3_add(dir, in.camera->up);
		}
		if (state[SDL_SCANCODE_C]) {
			dir = glms_vec3_sub(dir, in.camera->up);
		}

		$(in.camera, fly, dir, in.frameTime);

		$(queue, enqueue, drawScene, &in);
	}

	$(queue, stop);

	release(queue);

	SDL_DestroyWindow(in.window);

	SDL_Quit();

	return 0;
}

