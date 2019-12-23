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

#include <ObjectivelyGL/Buffer.h>

/**
 * @file
 * @brief VertexArrays facilitate the binding between generic vertex Attributes and Buffers.
 */

typedef struct VertexArray VertexArray;
typedef struct VertexArrayInterface VertexArrayInterface;

/**
 * @brief Attributes describe the elements of a VertexArray.
 */
typedef struct {
	/**
	 * @brief The index of the Attribute in the Shader inputs.
	 */
	GLuint index;

	/**
	 * @brief The size, in typed primitives, of the Attribute.
	 */
	GLint size;

	/**
	 * @brief The primitive type of the Attribute.
	 */
	GLenum type;

	/**
	 * @brief True if the Attribute is an integer type, and should be normalized to floating point.
	 */
	GLboolean normalized;

	/**
	 * @brief The stride of the Attribute in the VertexArray.
	 * @details Use `0` for tightly packed arrays, or `sizeof(VertexStruct)` for interleaved ones.
	 */
	GLsizei stride;

	/**
	 * @brief The offset pointer to the Attribute in the Shader inputs.
	 */
	const GLvoid *pointer;

} Attribute;

/**
 * @brief Creates an Attribute with the specified parameters.
 */
#define MakeAttribute(index, size, type, normalized, stride, pointer) \
	(Attribute) { (index), (size), (type), (normalized), (stride), (GLvoid *) (pointer) }

#define VertexAttribute(index, size, type, normalized, vertex, member) \
	MakeAttribute(index, size, type, normalized, sizeof(vertex), offsetof(vertex, member))

#define VertexAttributeVec2f(index, vertex, member) \
	VertexAttribute(index, 2, GL_FLOAT, GL_FALSE, vertex, member)

#define VertexAttributeVec3f(index, vertex, member) \
	VertexAttribute(index, 3, GL_FLOAT, GL_FALSE, vertex, member)

#define VertexAttributeVec4f(index, vertex, member) \
	VertexAttribute(index, 4, GL_FLOAT, GL_FALSE, vertex, member)

/**
 * @brief Creates a `NULL`-terminated array of Attributes.
 */
#define MakeAttributes(...) \
	{ \
		__VA_ARGS__, \
		MakeAttribute(0, 0, GL_NONE, GL_FALSE, 0, NULL) \
	}

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
