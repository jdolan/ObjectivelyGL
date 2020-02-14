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

	vec3s view;
	vec3s forward;
	vec3s right;
	vec3s up;

	vec3s light;

} View;

typedef struct {
	vec3s aPos;
	vec3s aNormal;
	vec3s aTexCoords;
	vec3s aTangent;
	vec2s aBitangent;
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

	in->model = $((Model *) alloc(WavefrontModel), initWithResourceName, "armor.obj");

	in->view = glms_vec3_scale(glms_vec3_add(in->model->mins, in->model->maxs), .5f);
	in->view.z = in->model->maxs.z * 1.5f;

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "parallax.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "parallax.fs.glsl")
	);

	in->program = $(alloc(Program), initWithDescriptor, &descriptor);
	FreeProgramDescriptor(&descriptor);

	const Attribute attributes[] = MakeAttributes(
		MakeVertexAttributeVec3f(TagPosition, 0, Vertex, aPos),
		MakeVertexAttributeVec3f(TagNormal, 1, Vertex, aNormal),
		MakeVertexAttributeVec3f(TagDiffuse, 1, Vertex, aTexCoords),
		MakeVertexAttributeVec3f(TagTangent, 1, Vertex, aTangent),
		MakeVertexAttributeVec3f(TagBitangent, 1, Vertex, aBitangent)
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
	const mat4s view = glms_lookat(in->view, GLMS_VEC3_ZERO, GLMS_YUP);
	const mat4s model = glms_euler_xyz(in->angles);

	$(in->program, setUniformForName, "projection", &projection);
	$(in->program, setUniformForName, "view", &view);
	$(in->program, setUniformForName, "model", &model);
	$(in->program, setUniformForName, "viewPos", &in->view);
	$(in->program, setUniformForName, "lightPos", &in->light);

	$(in->vertexArray, bind);
	$(in->vertexArray, enableAttribute, 0);
	$(in->vertexArray, enableAttribute, 1);
	$(in->vertexArray, enableAttribute, 2);
	$(in->vertexArray, enableAttribute, 3);
	$(in->vertexArray, enableAttribute, 4);

	$(in->elementsBuffer, bind, GL_ELEMENT_ARRAY_BUFFER);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawElements(GL_TRIANGLES, (GLsizei) in->model->elements->count, GL_UNSIGNED_INT, 0);

	printf("%d\n", glGetError());
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
				in.view.z -= event.wheel.y;
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
