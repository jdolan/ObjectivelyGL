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
 * @brief ProgramDescriptors provide a convenient way to initialize Programs from Resources.
 * @details Programs initialized with descriptors are automatically compiled and linked. Their
 * link status and information log, as well as the compilation status and information log of each
 * Shader, are made available on the descriptor.
 */
typedef struct {

	/**
	 * @brief The ShaderDescriptors to resolve, compile and link to this Program (required).
	 */
	ShaderDescriptor shaders[16];

	/**
	 * @brief The Program, if Shader compilation and linking was successful, or `NULL` on error.
	 */
	Program *program;

	/**
	 * @brief The link status; `GL_TRUE` on success, `GL_FALSE` on error.
	 */
	GLint status;

	/**
	 * @brief The information log.
	 */
	GLchar *infoLog;

} ProgramDescriptor;

/**
 * @brief Creates a ProgramDescriptor with the given ShaderDescriptor arguments.
 * @see MakeShaderDescriptor
 */
#define MakeProgramDescriptor(...) { \
	.shaders = { \
		__VA_ARGS__, \
		MakeShaderDescriptor(GL_NONE, NULL, NULL, GL_FALSE, NULL) \
	} \
}

/**
 * @brief Uniform variables.
 */
typedef struct {

	/**
	 * @brief The index.
	 */
	GLuint index;

	/**
	 * @brief The name.
	 */
	GLchar name[64];

	/**
	 * @brief The size, in units of type.
	 */
	GLint size;

	/**
	 * @brief The type, e.g. `GL_FLOAT_VEC3`.
	 */
	GLenum type;

} Variable;

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
	 * @fn Variable *Program::activeAttributes(const Program *self)
	 * @param self The Program.
	 * @return A _none-terminated_ array of this Program's attributes.
	 * @memberof Program
	 */
	Variable *(*activeAttributes)(const Program *self);

	/**
	 * @fn Variable *Program::activeUniforms(const Program *self)
	 * @param self The Program.
	 * @return A _none-terminated_ array of this Program's uniforms.
	 * @memberof Program
	 */
	Variable *(*activeUniforms)(const Program *self);

	/**
	 * @fn void Program::attach(Program *self, Shader *shader)
	 * @brief Attaches the specified Shader to this Program.
	 * @param self The Program.
	 * @param shader The Shader to attach.
	 * @memberof Program
	 */
	void (*attach)(Program *self, Shader *shader);

	/**
	 * @fn void Program::detach(Program *self, Shader *shader)
	 * @brief Detaches the specified Shader from this Program.
	 * @param self The Program.
	 * @param shader The Shader to detach.
	 * @memberof Program
	 */
	void (*detach)(Program *self, Shader *shader);

	/**
	 * @fn void Program::detachAll(Program *self)
	 * @brief Detaches all Shaders from this Program.
	 * @param self The Program.
	 * @param shader The Shader to detach.
	 * @memberof Program
	 */
	void (*detachAll)(Program *self);

	/**
	 * @fn GLchar *Program::infoLog(const Program *self)
	 * @param self The Program.
	 * @return The information log for the Program.
	 * @memberof Program
	 */
	GLchar *(*infoLog)(const Program *self);

	/**
	 * @fn Program *Program::init(Program *self)
	 * @brief Initializes this Program.
	 * @param self The Program.
	 * @return The initialized Program, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof Program
	 */
	Program *(*init)(Program *self);

	/**
	 * @fn Program *Program::initWithShaders(Program *self, ...)
	 * @brief Initializes this Program with the specified compiled Shaders.
	 * @param self The Program.
	 * @param ... A `NULL`-terminated list of one or more compiled Shaders.
	 * @return The initialized Program, or `NULL` on error.
	 * @memberof Program
	 */
	Program *(*initWithShaders)(Program *self, ...);

	/**
	 * @fn Program *Program::initWithDescriptor(Program *self, ProgramDescriptor *descriptor)
	 * @brief Initializes this Program with the specified ProgramDescriptor.
	 * @details All Shaders described by the ProgramDescriptor will be resolved and compiled.
	 * If all Shaders successfully resovlve and compile, the Program is linked. The link status
	 * and information log are written to the descriptor. If any Shaders fail to resolve or
	 * compile, the corresponding ShaderDescriptor will contain error information. The caller
	 * should call `FreeProgramDescriptor` when the descriptor is no longer needed.
	 * @param self The Program.
	 * @param descriptor The ProgramDescriptor.
	 * @return The initialized Program, or `NULL` on error.
	 * @memberof Program
	 */
	Program *(*initWithDescriptor)(Program *self, ProgramDescriptor *descriptor);
	
	/**
	 * @fn GLint Program::link(const Program *self)
	 * @brief Links this Program.
	 * @param self The Program.
	 * @return `GL_TRUE` on success, `GL_FALSE` on error.
	 * @memberof Program
	 */
	GLint (*link)(const Program *self);

	/**
	 * @fn void Program::use(const Program *self)
	 * @brief Installs this Program as part of the current rendering state.
	 * @param self The Program.
	 * @memberof Program
	 */
	void (*use)(const Program *self);
};

/**
 * @fn Class *Program::_Program(void)
 * @brief The Program archetype.
 * @return The Program Class.
 * @memberof Program
 */
OBJECTIVELYGL_EXPORT Class *_Program(void);

/**
 * @brief Frees a ProgramDescriptor when it is no longer needed.
 * @param descriptor The ProgramDescriptor.
 * @relates ProgramDescriptor
 */
OBJECTIVELYGL_EXPORT void FreeProgramDescriptor(ProgramDescriptor *descriptor);
