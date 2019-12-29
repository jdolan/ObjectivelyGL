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

#pragma once

#include <ObjectivelyGL/Buffer.h>

/**
 * @file
 * @brief UniformBuffers allow sharing of uniform Variables between Programs.
 */

typedef struct UniformBuffer UniformBuffer;
typedef struct UniformBufferInterface UniformBufferInterface;

/**
 * @brief Creates a BufferData suitable for use with a UniformBuffer.
 */
#define MakeUniformBufferData(size, data, usage) \
	MakeBufferData(GL_UNIFORM_BUFFER, size, data, usage)

/**
 * @brief Creates a BufferSubData suitable for use with a UniformBuffer.
 */
#define MakeUniformBufferSubData(offset, size, data) \
	MakeBufferSubData(GL_UNIFORM_BUFFER, offset, size, data)

/**
 * @brief The UniformBuffer type.
 * @extends Buffer
 */
struct UniformBuffer {

	/**
	 * @brief The superclass.
	 */
	Buffer buffer;

	/**
	 * @brief The interface.
	 * @protected
	 */
	UniformBufferInterface *interface;
};

/**
 * @brief The UniformBuffer interface.
 */
struct UniformBufferInterface {

	/**
	 * @brief The superclass interface.
	 */
	BufferInterface bufferInterface;

	/**
	 * @fn void Buffer::bindBase(const Buffer *self, GLuint index)
	 * @brief Binds this Buffer at the specified binding point.
	 * @param self The Buffer.
	 * @param index The binding point index.
	 * @memberof Buffer
	 */
	void (*bind)(const UniformBuffer *self, GLuint index);

	/**
	 * @fn void UniformBuffer::bindRange(const Buffer *self, GLuint index, GLintptr offset, GLsizeiptr size)
	 * @brief Binds this UniformBuffer at the specified binding point.
	 * @param self The UniformBuffer.
	 * @param index The binding point index.
	 * @param offset The offset to bind at the binding point.
	 * @param size The size of the data to bind at the binding point.
	 * @memberof Buffer
	 */
	void (*bindRange)(const UniformBuffer *self, GLuint index, GLintptr offset, GLsizeiptr size);

	/**
	 * @fn UniformBuffer *UniformBuffer::init(UniformBuffer *self)
	 * @brief Initializes this UniformBuffer.
	 * @param self The UniformBuffer.
	 * @return The initialized UniformBuffer, or `NULL` on error.
	 * @memberof UniformBuffer
	 */
	UniformBuffer *(*init)(UniformBuffer *self);

	/**
	 * @fn Buffer *UniformBuffer::initWithData(UniformBuffer *self, const BufferData *data)
	 * @brief Initializes this UniformBuffer.
	 * @param self The UniformBuffer.
	 * @param data The BufferData.
	 * @return The initialized Buffer, or `NULL` on error.
	 * @memberof Buffer
	 */
	UniformBuffer *(*initWithData)(UniformBuffer *self, const BufferData *data);
};

/**
 * @fn Class *UniformBuffer::_UniformBuffer(void)
 * @brief The UniformBuffer archetype.
 * @return The UniformBuffer Class.
 * @memberof UniformBuffer
 */
OBJECTIVELYGL_EXPORT Class *_UniformBuffer(void);
