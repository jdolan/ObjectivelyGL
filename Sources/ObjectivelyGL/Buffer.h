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

#include <Objectively/Object.h>

#include <ObjectivelyGL/Types.h>

/**
 * @file
 * @brief Buffers are contiguous arrays of GPU memory used to communicate with Programs.
 * @details Buffers can contain vertex data, pixel data retrieved from framebuffers, etc.
 */

typedef struct Buffer Buffer;
typedef struct BufferInterface BufferInterface;

/**
 * @brief BufferData is used to initialize or alter the storage and data of a Buffer.
 */
typedef struct {

	/**
	 * @brief The target.
	 */
	GLenum target;

	/**
	 * @brief The requested size of the Buffer's data store, in bytes.
	 */
	GLsizeiptr size;

	/**
	 * @brief The data.
	 */
	const ident data;

	/**
	 * @brief The usage hint.
	 */
	GLenum usage;

} BufferData;

/**
 * @brief Creates a BufferData with the specified parameters.
 */
#define MakeBufferData(target, size, data, usage) \
	(BufferData) { (target), (size), (ident) (data), (usage) }

/**
 * @brief BufferSubData is used to modify the data of a Buffer.
 */
typedef struct {

	/**
	 * @brief The target.
	 */
	GLenum target;

	/**
	 * @brief The offset of the sub-data operation, in bytes.
	 */
	GLsizeiptr offset;

	/**
	 * @brief The size of the sub-data operation, in bytes.
	 */
	GLsizeiptr size;

	/**
	 * @brief The data.
	 */
	const ident data;

} BufferSubData;

/**
 * @brief Creates a BufferSubData with the specified parameters.
 */
#define MakeBufferSubData(target, offset, size, data) \
	(BufferSubData) { (target), (offset), (size), (ident) (data) }

/**
 * @brief The Buffer type.
 * @extends Object
 */
struct Buffer {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	BufferInterface *interface;

	/**
	 * @brief The Buffer name.
	 */
	GLuint name;


	/**
	 * @brief The size of the Buffer's data store, in bytes.
	 */
	GLsizeiptr size;

	/**
	 * @brief The usage hint.
	 */
	GLenum usage;
};

/**
 * @brief The Buffer interface.
 */
struct BufferInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Buffer::bind(const Buffer *self, GLenum target)
	 * @brief Binds this Buffer for the specified target.
	 * @param self The Buffer.
	 * @param target The target, e.g. `GL_ARRAY_BUFFER`, `GL_ELEMENT_ARRAY_BUFFER`, etc.
	 * @memberof Buffer
	 */
	void (*bind)(Buffer *self, GLenum target);

	/**
	 * @fn Buffer *Buffer::init(Buffer *self)
	 * @brief Initializes this Buffer.
	 * @param self The Buffer.
	 * @return The initialized Buffer, or `NULL` on error.
	 * @memberof Buffer
	 */
	Buffer *(*init)(Buffer *self);

	/**
	 * @fn Buffer *Buffer::initWithData(Buffer *self, const BufferData *data)
	 * @brief Initializes this Buffer.
	 * @param self The Buffer.
	 * @param data The BufferData.
	 * @return The initialized Buffer, or `NULL` on error.
	 * @memberof Buffer
	 */
	Buffer *(*initWithData)(Buffer *self, const BufferData *data);

	/**
	 * @fn void Buffer::unbind(const Buffer *self, GLenum target)
	 * @brief Unbinds this Buffer from the specified target.
	 * @param self The Buffer.
	 * @param target The target, e.g. `GL_ARRAY_BUFFER`, `GL_ELEMENT_ARRAY_BUFFER`, etc.
	 * @memberof Buffer
	 */
	void (*unbind)(const Buffer *self, GLenum target);

	/**
	 * @fn void Buffer::writeData(const Buffer *self, const BufferData *data)
	 * @brief Writes data to this this Buffer's data store, (re) allocating storage.
	 * @details The Buffer is (re)allocated if necessary.
	 * @param self The Buffer.
	 * @param data The BufferData.
	 * @memberof Buffer
	 */
	void (*writeData)(Buffer *self, const BufferData *data);

	/**
	 * @fn void Buffer::writeSubData(const Buffer *self, const BufferData *data)
	 * @brief Writes sub-data to this this Buffer's data store.
	 * @details The specified data must fit within this Buffer.
	 * @param self The Buffer.
	 * @param data The BufferData.
	 * @memberof Buffer
	 */
	void (*writeSubData)(Buffer *self, const BufferSubData *data);
};

/**
 * @fn Class *Buffer::_Buffer(void)
 * @brief The Buffer archetype.
 * @return The Buffer Class.
 * @memberof Buffer
 */
OBJECTIVELY_EXPORT Class *_Buffer(void);
