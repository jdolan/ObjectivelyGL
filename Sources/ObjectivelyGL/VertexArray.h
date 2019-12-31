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

#include <ObjectivelyGL/Attribute.h>
#include <ObjectivelyGL/Buffer.h>

/**
 * @file
 * @brief VertexArrays facilitate the binding between generic vertex Attributes and Buffers.
 */

typedef struct VertexArray VertexArray;
typedef struct VertexArrayInterface VertexArrayInterface;

/**
 * @brief The VertexArray type.
 * @extends Object
 */
struct VertexArray {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	VertexArrayInterface *interface;

	/**
	 * @brief The name.
	 */
	GLuint name;

	/**
	 * @brief The Attributes.
	 */
	Attribute *attributes;

	/**
	 * @brief The Buffer providing the generic vertex data.
	 */
	Buffer *buffer;
};

/**
 * @brief The VertexArray interface.
 */
struct VertexArrayInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void VertexArray::bind(const VertexArray *self)
	 * @brief Binds this VertexArray to the current context.
	 * @param self The VertexArray.
	 * @memberof VertexArray
	 */
	void (*bind)(const VertexArray *self);

	/**
	 * @fn void VertexArray::enableAttribute(VertexArray *self, GLuint index)
	 * @brief Enables the Attribute at the given index.
	 * @param self The VertexArray.
	 * @param index The index.
	 * @memberof VertexArray
	 */
	void (*enableAttribute)(VertexArray *self, GLuint index);

	/**
	 * @fn void VertexArray::disableAttribute(VertexArray *self, GLuint index)
	 * @brief Disables the Attribute at the given index.
	 * @param self The VertexArray.
	 * @param index The index.
	 * @memberof VertexArray
	 */
	void (*disableAttribute)(VertexArray *self, GLuint index);

	/**
	 * @fn VertexArray *VertexArray::initWithAttributes(VertexArray *self, Buffer *buffer, const Attribute *attributes)
	 * @brief Initializes this VertexArray with the backing Buffer and Attributes.
	 * @param self The VertexArray.
	 * @param buffer The backing Buffer, which will be bound to `GL_ARRAY_BUFFER`.
	 * @param attributes The Attributes.
	 * @return The initialized VertexArray, or `NULL` on error.
	 * @memberof VertexArray
	 */
	VertexArray *(*initWithAttributes)(VertexArray *self, Buffer *buffer, const Attribute *attributes);

	/**
	 * @fn void VertexArray::unbind(const VertexArray *self)
	 * @brief Unbinds this VertexArray from the current context.
	 * @param self The VertexArray.
	 * @memberof VertexArray
	 */
	void (*unbind)(const VertexArray *self);
};

/**
 * @fn Class *VertexArray::_VertexArray(void)
 * @brief The VertexArray archetype.
 * @return The VertexArray Class.
 * @memberof VertexArray
 */
OBJECTIVELYGL_EXPORT Class *_VertexArray(void);
