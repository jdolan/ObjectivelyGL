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
#include <Objectively/Vector.h>

#include <ObjectivelyGL/Shader.h>

/**
 * @file
 * @brief A Program is an executable comprised of one or more compiled Shaders.
 */

typedef struct Program Program;
typedef struct ProgramInterface ProgramInterface;

typedef void (*ProgramUse)(const Program *self);

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
 * @brief Uniform and attribute variables.
 */
typedef struct {

	/**
	 * @brief The index.
	 */
	GLuint index;

	/**
	 * @brief The location.
	 */
	GLint location;

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
 * @brief Uniform blocks.
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
	 * @brief The binding point.
	 */
	GLint binding;

} UniformBlock;

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
	 * @brief The attribute Variables defined by this Program.
	 */
	Vector *attributes;

	/**
	 * @brief The uniform Variables defined by this Program.
	 */
	Vector *uniforms;

	/**
	 * @brief The UniformBlocks defined by this Program.
	 */
	Vector *uniformBlocks;

	/**
	 * @brief An optional callback that, if set, is called on `use`.
	 */
	ProgramUse use;

	/**
	 * @brief User data.
	 */
	ident data;
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
	 * @fn void Program::attach(Program *self, Shader *shader)
	 * @brief Attaches the specified Shader to this Program.
	 * @param self The Program.
	 * @param shader The Shader to attach.
	 * @memberof Program
	 */
	void (*attach)(Program *self, Shader *shader);

	/**
	 * @fn const Variable *Program::attributeForName(const Program *self, const GLchar *name)
	 * @param self The Program.
	 * @param name The attribute Variable name.
	 * @return The attribute Variable for the given name, or `NULL`.
	 * @memberof Program
	 */
	const Variable *(*attributeForName)(const Program *self, const GLchar *name);

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
	 * @fn GLint Program::link(Program *self)
	 * @brief Links this Program and resolves all attribute and uniform Variables.
	 * @param self The Program.
	 * @return `GL_TRUE` on success, `GL_FALSE` on error.
	 * @memberof Program
	 */
	GLint (*link)(Program *self);

	/**
	 * @fn void Program::setUniform(const Program *self, const Variable *variable, const GLvoid *value)
	 * @brief Sets the uniform Variable to the specified value.
	 * @param self The Program.
	 * @param variable The uniform Variable.
	 * @param value The value, which must be a pointer.
	 * @memberof Program
	 */
	void (*setUniform)(const Program *self, const Variable *variable, const GLvoid *value);

	/**
	 * @fn void Program::setUniformBlockBinding(const Program *self, const UniformBlock *block, GLint index)
	 * @brief Sets the UniformBlock binding to the specified index.
	 * @param self The Program.
	 * @param block The UniformBlock.
	 * @param index The binding index, which must be less than the value of `GL_MAX_UNIFORM_BUFFER_BINDINGS`.
	 * @memberof Program
	 */
	void (*setUniformBlockBinding)(const Program *self, const UniformBlock *block, GLuint index);

	/**
	 * @fn void Program::setUniformBlockBindingForName(const Program *self, const GLchar *name, GLint index)
	 * @brief Sets the UniformBlock binding for the given name to the specified index.
	 * @param self The Program.
	 * @param name The UniformBlock name.
	 * @param index The binding index, which must be less than the value of `GL_MAX_UNIFORM_BUFFER_BINDINGS`.
	 * @memberof Program
	 */
	void (*setUniformBlockBindingForName)(const Program *self, const GLchar *name, GLuint index);

	/**
	 * @fn void Program::setUniformForName(const Program *self, const GLchar *name, const void *value)
	 * @brief Sets the uniform Variable for the given name to the specified value.
	 * @param self The Program.
	 * @param name The uniform Variable name.
	 * @param value The value, which must be a pointer.
	 * @memberof Program
	 */
	void (*setUniformForName)(const Program *self, const GLchar *name, const void *value);

	/**
	 * @fn const UniformBlock *Program::uniformBlockForName(const Program *self, const GLchar *name)
	 * @param self The Program.
	 * @param name The UniformBlock name.
	 * @return The UniformBlock for the given name, or `NULL`.
	 * @memberof Program
	 */
	const UniformBlock *(*uniformBlockForName)(const Program *self, const GLchar *name);

	/**
	 * @fn const Variable *Program::uniformForName(const Program *self, const GLchar *name)
	 * @param self The Program.
	 * @param name The uniform Variable name.
	 * @return The uniform Variable for the given name, or `NULL`.
	 * @memberof Program
	 */
	const Variable *(*uniformForName)(const Program *self, const GLchar *name);
	
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
