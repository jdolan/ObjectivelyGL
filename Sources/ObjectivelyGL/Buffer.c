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

#include "Buffer.h"

#define _Class _Buffer

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Buffer *this = (Buffer *) self;

	glDeleteBuffers(1, &this->name);

	super(Object, self, dealloc);
}

#pragma mark - Buffer

/**
 * @fn void Buffer::bind(Buffer *self, GLenum target)
 * @memberof Buffer
 */
static void bind(Buffer *self, GLenum target) {

	glBindBuffer(target, self->name);
}

/**
 * @fn Buffer *Buffer::init(Buffer *self)
 * @memberof Buffer
 */
static Buffer *init(Buffer *self) {

	self = (Buffer *) super(Object, self, init);
	if (self) {
		glGenBuffers(1, &self->name);
		if (self->name) {

		} else {
			self = release(self);
		}
	}

	return self;
}

/**
 * @fn Buffer *Buffer::initWithData(Buffer *self, const BufferData *data)
 * @memberof Buffer
 */
static Buffer *initWithData(Buffer *self, const BufferData *data) {

	self = $(self, init);
	if (self) {
		$(self, bind, data->target);
		$(self, writeData, data);
	}

	return self;
}

/**
 * @fn void Buffer::writeData(Buffer *self, const BufferData *data)
 * @memberof Buffer
 */
static void writeData(Buffer *self, const BufferData *data) {

	glBufferData(data->target, data->size, data->data, data->usage);

	self->size = data->size;
	self->usage = data->usage;
}

/**
 * @fn void Buffer::writeSubData(Buffer *self, const BufferSubData *data)
 * @memberof Buffer
 */
static void writeSubData(Buffer *self, const BufferSubData *data) {

	glBufferSubData(data->target, data->offset, data->size, data->data);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((BufferInterface *) clazz->interface)->bind = bind;
	((BufferInterface *) clazz->interface)->init = init;
	((BufferInterface *) clazz->interface)->initWithData = initWithData;
	((BufferInterface *) clazz->interface)->writeData = writeData;
	((BufferInterface *) clazz->interface)->writeSubData = writeSubData;
}

/**
 * @fn Class *Buffer::_Buffer(void)
 * @memberof Buffer
 */
Class *_Buffer(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Buffer",
			.superclass = _Object(),
			.instanceSize = sizeof(Buffer),
			.interfaceOffset = offsetof(Buffer, interface),
			.interfaceSize = sizeof(BufferInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
