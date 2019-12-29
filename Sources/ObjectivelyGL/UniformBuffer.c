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

#include <ObjectivelyGL/UniformBuffer.h>

#define _Class _UniformBuffer

#pragma mark - UniformBuffer

/**
 * @fn void UniformBuffer::bind(const UniformBuffer *self, GLenum target, GLuint index)
 * @memberof UniformBuffer
 */
static void bind(const UniformBuffer *self, GLuint index) {
	glBindBufferBase(GL_UNIFORM_BUFFER, index, self->buffer.name);
}

/**
 * @fn void UniformBuffer::bindRange(const UniformBuffer *self, GLuint index, GLintptr offset, GLsizeiptr size)
 * @memberof UniformBuffer
 */
static void bindRange(const UniformBuffer *self, GLuint index, GLintptr offset, GLsizeiptr size) {
	glBindBufferRange(GL_UNIFORM_BUFFER, index, self->buffer.name, offset, size);
}

/**
 * @fn UniformBuffer *UniformBuffer::init(UniformBuffer *self)
 * @memberof UniformBuffer
 */
static UniformBuffer *init(UniformBuffer *self) {

	self = (UniformBuffer *) super(Buffer, self, init);
	if (self) {

		//..
	}

	return self;
}

/**
 * @fn UniformBuffer *UniformBuffer::initWithData(UniformBuffer *self, const BufferData *data)
 * @memberof UniformBuffer
 */
static UniformBuffer *initWithData(UniformBuffer *self, const BufferData *data) {

	self = (UniformBuffer *) super(Buffer, self, initWithData, data);
	if (self) {

		//..
	}

	return self;
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((UniformBufferInterface *) clazz->interface)->bind = bind;
	((UniformBufferInterface *) clazz->interface)->bindRange = bindRange;
	((UniformBufferInterface *) clazz->interface)->init = init;
	((UniformBufferInterface *) clazz->interface)->initWithData = initWithData;
}

/**
 * @fn Class *UniformBuffer::_UniformBuffer(void)
 * @memberof UniformBuffer
 */
Class *_UniformBuffer(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "UniformBuffer",
			.superclass = _Buffer(),
			.instanceSize = sizeof(UniformBuffer),
			.interfaceOffset = offsetof(UniformBuffer, interface),
			.interfaceSize = sizeof(UniformBufferInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
