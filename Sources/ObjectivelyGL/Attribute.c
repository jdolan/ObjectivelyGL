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

#include "Attribute.h"

/**
 * @brief
 */
OBJECTIVELYGL_EXPORT size_t SizeOfAttribute(const Attribute *attribute) {

	size_t size = 0;
	switch (attribute->type) {
		case GL_FLOAT:
			size = sizeof(GLfloat);
			break;
		case GL_BYTE:
		case GL_UNSIGNED_BYTE:
			size = sizeof(GLbyte);
			break;
		case GL_INT:
		case GL_UNSIGNED_INT:
			size = sizeof(GLint);
			break;
		default:
			break;
	}

	return size * attribute->size;
}

OBJECTIVELYGL_EXPORT size_t SizeOfAttributes(const Attribute *attributes) {

	size_t size = 0;
	while (attributes->type != GL_NONE) {
		size += SizeOfAttribute(attributes);
		attributes++;
	}

	return size;
}
