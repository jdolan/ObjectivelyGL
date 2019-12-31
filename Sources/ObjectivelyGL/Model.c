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

#include "Model.h"

#define _Class _Model

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Model *this = (Model *) self;

	for (size_t i = 0; i < this->meshes->count; i++) {
		free(VectorElement(this->meshes, ModelMesh, i)->name);
	}

	release(this->elements);
	release(this->meshes);
	release(this->vertices);

	super(Object, self, dealloc);
}

#pragma mark - Model

/**
 * @fn Buffer *Model::elementsBuffer(const Model *self)
 * @memberof Model
 */
static Buffer *elementsBuffer(const Model *self) {

	const BufferData data = MakeBufferData(GL_ELEMENT_ARRAY_BUFFER,
										   self->elements->count * self->elements->size,
										   self->elements->elements,
										   GL_STATIC_DRAW);

	return $(alloc(Buffer), initWithData, &data);
}

/**
 * @fn Model *Model::init(Model *self)
 * @memberof Model
 */
static Model *init(Model *self) {

	self = (Model *) super(Object, self, init);
	if (self) {
		self->elements = $(alloc(Vector), initWithSize, sizeof(GLuint));
		assert(self->elements);

		self->meshes = $(alloc(Vector), initWithSize, sizeof(ModelMesh));
		assert(self->meshes);

		self->vertices = $(alloc(Vector), initWithSize, sizeof(ModelVertex));
		assert(self->vertices);
	}

	return self;
}

/**
 * @fn Model *Model::initWithBytes(Model *self, const uint8_t *bytes, size_t length)
 * @memberof Model
 */
static Model *initWithBytes(Model *self, const uint8_t *bytes, size_t length) {

	self = $(self, init);
	if (self) {
		$(self, load, bytes, length);
	}

	return self;
}

/**
 * @fn Model *Model::initWithResource(Model *self, const Data *data)
 * @memberof Model
 */
static Model *initWithData(Model *self, const Data *data) {

	if (data) {
		return $(self, initWithBytes, data->bytes, data->length);
	} else {
		return release(self);
	}
}

/**
 * @fn Model *Model::initWithResource(Model *self, const Resource *resource)
 * @memberof Model
 */
static Model *initWithResource(Model *self, const Resource *resource) {

	if (resource) {
		return $(self, initWithData, resource->data);
	} else {
		return release(self);
	}
}

/**
 * @fn Model *Model::initWithResourceName(Model *self, const char *name)
 * @memberof Model
 */
static Model *initWithResourceName(Model *self, const char *name) {

	Resource *resource = $(alloc(Resource), initWithName, name);
	if (resource) {
		self = $(self, initWithResource, resource);
	} else {
		self = release(self);
	}
	release(resource);
	return self;
}

/**
 * @fn void Model::load(Model *self, const uint8_t *bytes, size_t length)
 * @memberof Model
 */
static void load(Model *self, const uint8_t *bytes, size_t length) {

}

/**
 * @fn VertexArray *Model::vertexArray(const Model *self, const Attribute *attributes)
 * @memberof Model
 */
static VertexArray *vertexArray(const Model *self, const Attribute *attributes) {

	VertexArray *vertexArray;

	Buffer *buffer = $(self, vertexBuffer, attributes);
	if (buffer) {
		vertexArray = $(alloc(VertexArray), initWithAttributes, buffer, attributes);
	} else {
		vertexArray = NULL;
	}
	release(buffer);

	return vertexArray;
}

/**
 * @fn Buffer *Model::vertexBuffer(const Model *self, const Attribute *attributes)
 * @memberof Model
 */
static Buffer *vertexBuffer(const Model *self, const Attribute *attributes) {

	const size_t vertexSize = SizeOfAttributes(attributes);
	ident vertices = malloc(vertexSize * self->vertices->count);
	ident out = vertices;

	ModelVertex *in = self->vertices->elements;
	for (size_t i = 0; i < self->vertices->count; i++, in++, out += vertexSize) {

		const Attribute *attr = attributes;
		while (attr->type != GL_NONE) {

			ident dest = out + (ptrdiff_t) attr->pointer;
			const size_t size = SizeOfAttributes(attr);

			switch (attr->tag) {
				case TagPosition:
					memcpy(dest, &in->position, size);
					break;
				case TagColor:
					memcpy(dest, &in->color, size);
					break;
				case TagDiffuse:
					memcpy(dest, &in->diffuse, size);
					break;
				case TagLightmap:
					memcpy(dest, &in->lightmap, size);
					break;
				case TagNormal:
					memcpy(dest, &in->normal, size);
					break;
				case TagTangent:
					memcpy(dest, &in->tangent, size);
					break;
				case TagBitangent:
					memcpy(dest, &in->bitangent, size);
					break;
				default:
					break;
			}

			attr++;
		}
	}

	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER,
										   vertexSize * self->vertices->count,
										   vertices,
										   GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), initWithData, &data);

	free(vertices);

	return buffer;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ModelInterface *) clazz->interface)->elementsBuffer = elementsBuffer;
	((ModelInterface *) clazz->interface)->init = init;
	((ModelInterface *) clazz->interface)->initWithBytes = initWithBytes;
	((ModelInterface *) clazz->interface)->initWithData = initWithData;
	((ModelInterface *) clazz->interface)->initWithResource = initWithResource;
	((ModelInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
	((ModelInterface *) clazz->interface)->load = load;
	((ModelInterface *) clazz->interface)->vertexArray = vertexArray;
	((ModelInterface *) clazz->interface)->vertexBuffer = vertexBuffer;
}

/**
 * @fn Class *Model::_Model(void)
 * @memberof Model
 */
Class *_Model(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Model",
			.superclass = _Object(),
			.instanceSize = sizeof(Model),
			.interfaceOffset = offsetof(Model, interface),
			.interfaceSize = sizeof(ModelInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
