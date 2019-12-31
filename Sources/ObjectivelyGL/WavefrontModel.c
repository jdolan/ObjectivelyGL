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
#include <string.h>

#include "WavefrontModel.h"

#define _Class _WavefrontModel

typedef struct {
	char *file;
	Vector *v;
	Vector *vt;
	Vector *vn;
} Wavefront;

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	//..

	super(Object, self, dealloc);
}

#pragma mark - Model

/**
 * @brief Finds or adds a MeshVertex with the given indices to this model.
 * @return The vertex index.
 */
static GLuint findOrAddVertex(Model *self, const Wavefront *obj, const ivec3s indices) {

	ModelVertex vertex = { 0 };
	if (indices.x) {
		vertex.position = *VectorElement(obj->v, vec3s, indices.x - 1);
	}
	if (indices.y) {
		vertex.diffuse = *VectorElement(obj->vt, vec2s, indices.y - 1);
	}
	if (indices.z) {
		vertex.normal = *VectorElement(obj->vn, vec3s, indices.z - 1);
	}

	ssize_t index = $(self->vertices, indexOfElement, &vertex);
	if (index == -1) {
		$(self->vertices, addElement, &vertex);
		index = self->vertices->count - 1;
	}

	return (GLuint) index;
}

/**
 * @brief Perform post processing on the given MeshVertex.
 */
static void postProcessVertex(const Vector *vector, ident obj, ident data) {

	ModelVertex *vertex = obj;
	glms_vec3_normalize(vertex->normal);
}

/**
 * @fn Model *Model::initWithResource(Model *self, const uint8_t *bytes, size_t length)
 * @memberof Model
 */
static Model *initWithBytes(Model *self, const uint8_t *bytes, size_t length) {

	self = super(Model, self, initWithBytes, bytes, length);
	if (self) {

		Wavefront obj = {
			.file = calloc(length + 1, sizeof(char)),
			.v = $(alloc(Vector), initWithSize, sizeof(vec3s)),
			.vt = $(alloc(Vector), initWithSize, sizeof(vec2s)),
			.vn = $(alloc(Vector), initWithSize, sizeof(vec3s)),
		};

		memcpy(obj.file, bytes, length);

		ModelMesh mesh = {
			.type = GL_TRIANGLES
		};

		for (char *line = strtok(obj.file, "\r\n"); line; line = strtok(NULL, "\r\n")) {

			vec3s vec;
			if (strncmp("v ", line, strlen("v ")) == 0) {
				if (sscanf(line, "v %f %f %f", &vec.x, &vec.y, &vec.z) == 3) {
					$(obj.v, addElement, &vec);
				}
			} else if (strncmp("vt ", line, strlen("vt ")) == 0) {
				if (sscanf(line, "vt %f %f", &vec.x, &vec.y) == 2) {
					$(obj.vt, addElement, &vec);
				}
			} else if (strncmp("vn ", line, strlen("vn ")) == 0) {
				if (sscanf(line, "vn %f %f %f", &vec.x, &vec.y, &vec.z) == 3) {
					$(obj.vn, addElement, &vec);
				}
			} else if (strncmp("g ", line, strlen("g ")) == 0) {
				if (mesh.count) {
					$(self->meshes, addElement, &mesh);
				}
				mesh = (ModelMesh) {
					.name = strdup(line + strlen("g ")),
					.type = GL_TRIANGLES
				};
			} else if (strncmp("f ", line, strlen("f ")) == 0) {

				if (mesh.count == 0) {
					mesh.elements = self->elements->count;
				}

				Vector *face = $(alloc(Vector), initWithSize, sizeof(GLuint));

				char *token = line + 1;
				while (*token) {

					ivec3s indices = { 0, 0, 0 };
					indices.x = (int) strtol(token + 1, &token, 10);
					if (*token == '/') {
						indices.y = (int) strtol(token + 1, &token, 10);
						if (*token == '/') {
							indices.z = (int) strtol(token + 1, &token, 10);
						}
					} else if (indices.x == 0) {
						break;
					}

					const GLuint element = findOrAddVertex(self, &obj, indices);
					$(face, addElement, (ident) &element);
				}

				for (size_t i = 2; i < face->count; i++) {
					const GLuint *a = VectorElement(face, GLuint, 0);
					const GLuint *b = VectorElement(face, GLuint, i - 1);
					const GLuint *c = VectorElement(face, GLuint, i);

					$(self->elements, addElement, (ident) a);
					$(self->elements, addElement, (ident) b);
					$(self->elements, addElement, (ident) c);

					mesh.count += 3;
				}

				release(face);
			}
		}

		$(self->meshes, addElement, &mesh);

		$(self->vertices, enumerateElements, postProcessVertex, NULL);

		free(obj.file);
		release(obj.v);
		release(obj.vt);
		release(obj.vn);
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ModelInterface *) clazz->interface)->initWithBytes = initWithBytes;
}

/**
 * @fn Class *WavefrontModel::_WavefrontModel(void)
 * @memberof WavefrontModel
 */
Class *_WavefrontModel(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "WavefrontModel",
			.superclass = _Model(),
			.instanceSize = sizeof(WavefrontModel),
			.interfaceOffset = offsetof(WavefrontModel, interface),
			.interfaceSize = sizeof(WavefrontModelInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
