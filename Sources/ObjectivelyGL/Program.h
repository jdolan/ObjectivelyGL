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

#include <Objectively/MutableArray.h>

#include <ObjectivelyGL/Shader.h>

/**
 * @file
 * @brief A Program is an executable comprised of one or more compiled Shaders.
 */

typedef struct Program Program;
typedef struct ProgramInterface ProgramInterface;

/**
 * @brief The Program type.
 * @extends Object
 */
struct Program {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ProgramInterface *interface;

	/**
	 * @brief The program name.
	 */
	GLuint name;

	/**
	 * @brief The Shaders attached to this Program.
	 */
	MutableArray *shaders;

	/**
	 * @brief The info log, available after linking.
	 */
	GLchar *info;
};

/**
 * @brief The Program interface.
 */
struct ProgramInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Program *Program::initWithShaders(Program *self, ...)
	 * @brief Initializes this Program.
	 * @param self The Program.
	 * @return The initialized Program, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof Program
	 */
	Program *(*init)(Program *self);

	/**
	 * @fn Program *Program::initWithShaders(Program *self, ...)
	 * @brief Initializes this Program with the specified Shaders.
	 * @param self The Program.
	 * @param ... A `NULL`-terminated list of one or more Shaders.
	 * @return The initialized Program, or `NULL` on error.
	 * @memberof Program
	 */
	Program *(*initWithShaders)(Program *self, ...);

	/**
	 * @fn Program *Program::initWithDescriptors(Program *self, ShaderDescriptor *descriptors)
	 * @brief Initializes this Program with the specified ShaderDescriptors.
	 * @param self The Program.
	 * @param descriptors A `NULL`-terminated array of one or more ShaderDescriptors.
	 * @return The initialized Program, or `NULL` on error.
	 * @memberof Program
	 */
	Program *(*initWithDescriptors)(Program *self, ShaderDescriptor *descriptors);

	/**
	 * @fn GLint Program::link(Program *self)
	 * @brief Links this Program.
	 * @param self The Program.
	 * @return `GL_TRUE` on success, `GL_FALSE` on error.
	 * @memberof Program
	 */
	GLint (*link)(Program *self);
};

/**
 * @fn Class *Program::_Program(void)
 * @brief The Program archetype.
 * @return The Program Class.
 * @memberof Program
 */
OBJECTIVELYGL_EXPORT Class *_Program(void);
