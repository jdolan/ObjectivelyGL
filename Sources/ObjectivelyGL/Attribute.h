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

#include <ObjectivelyGL/Types.h>

/**
 * @brief Tags provide convenient binding of Model data to array Attributes.
 */
typedef enum {
	TagNone,
	TagPosition,
	TagNormal,
	TagTangent,
	TagBitangent,
	TagDiffuse,
	TagLightmap,
	TagColor,
} AttributeTag;

/**
 * @brief Attributes describe the elements of a VertexArray.
 */
typedef struct {

	/**
	 * @brief The tag.
	 */
	AttributeTag tag;

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
#define MakeAttribute(tag, index, size, type, normalized, stride, pointer) \
	(Attribute) { (tag), (index), (size), (type), (normalized), (stride), (GLvoid *) (pointer) }

#define MakeVertexAttribute(tag, index, size, type, normalized, vertex, member) \
	MakeAttribute(tag, index, size, type, normalized, sizeof(vertex), offsetof(vertex, member))

#define MakeVertexAttributeVec2f(tag, index, vertex, member) \
	MakeVertexAttribute(tag, index, 2, GL_FLOAT, GL_FALSE, vertex, member)

#define MakeVertexAttributeVec3f(tag, index, vertex, member) \
	MakeVertexAttribute(tag, index, 3, GL_FLOAT, GL_FALSE, vertex, member)

#define MakeVertexAttributeVec4f(tag, index, vertex, member) \
	MakeVertexAttribute(tag, index, 4, GL_FLOAT, GL_FALSE, vertex, member)

#define MakeVertexAttributeVec4ub(tag, index, vertex, member) \
	MakeVertexAttribute(tag, index, 4, GL_UNSIGNED_BYTE, GL_FALSE, vertex, member)

/**
 * @brief Creates a `NULL`-terminated array of Attributes.
 */
#define MakeAttributes(...) \
	{ \
		__VA_ARGS__, \
		MakeAttribute(0, 0, 0, GL_NONE, GL_FALSE, 0, NULL) \
	}

/**
 * @return The size of the specified Attribute in bytes.
 */
OBJECTIVELYGL_EXPORT size_t SizeOfAttribute(const Attribute *attribute);

/**
 * @return The size of the specified `GL_NONE`-terminated Attributes in bytes.
 */
OBJECTIVELYGL_EXPORT size_t SizeOfAttributes(const Attribute *attributes);
