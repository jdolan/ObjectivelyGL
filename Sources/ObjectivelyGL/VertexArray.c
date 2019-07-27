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

#include "VertexArray.h"

#define _Class _VertexArray

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	VertexArray *this = (VertexArray *) self;

	glDeleteVertexArrays(1, &this->name);

	super(Object, self, dealloc);
}

#pragma mark - VertexArray

static void attributePointers(VertexArray *self, const Attribute *attrs) {

	while (attrs->type != GL_NONE) {
		glVertexAttribPointer(attrs->index,
							  attrs->size,
							  attrs->type,
							  attrs->normalized,
							  attrs->stride,
							  attrs->pointer);
		attrs++;
	}
}

static void bind(const VertexArray *self) {
	glBindVertexArray(self->name);
}

static void enableAttribute(VertexArray *self, GLuint index) {
	glEnableVertexAttribArray(index);
}

/**
 * @fn VertexArray *VertexArray::init(VertexArray *self)
 * @memberof VertexArray
 */
static VertexArray *init(VertexArray *self) {

	self = (VertexArray *) super(Object, self, init);
	if (self) {
		glGenVertexArrays(1, &self->name);
		if (self->name) {

		} else {
			self = release(self);
		}
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((VertexArrayInterface *) clazz->interface)->attributePointers = attributePointers;
	((VertexArrayInterface *) clazz->interface)->bind = bind;
	((VertexArrayInterface *) clazz->interface)->enableAttribute = enableAttribute;
	((VertexArrayInterface *) clazz->interface)->init = init;
}

/**
 * @fn Class *VertexArray::_VertexArray(void)
 * @memberof VertexArray
 */
Class *_VertexArray(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "VertexArray",
			.superclass = _Object(),
			.instanceSize = sizeof(VertexArray),
			.interfaceOffset = offsetof(VertexArray, interface),
			.interfaceSize = sizeof(VertexArrayInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
