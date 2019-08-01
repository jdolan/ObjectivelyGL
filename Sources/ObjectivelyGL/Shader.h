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

#include <Objectively/Resource.h>

#include <ObjectivelyGL/Types.h>

/**
 * @file
 * @brief A Shader is a typed portion of a Program comprised of GLSL source code.
 */

typedef struct Shader Shader;
typedef struct ShaderInterface ShaderInterface;

/**
 * @brief ShaderDescriptors provide a convenient way to initialize Shaders from Resources.
 */
typedef struct {

	/**
	 * @brief The Shader type (required).
	 */
	GLenum type;

	/**
	 * @brief The names of Resources containing Shader source (required).
	 */
	const char *resources[16];

	/**
	 * @brief The Shader, if initialization was successful, or `NULL` on error.
	 */
	Shader *shader;

	/**
	 * @brief The compilation status; `GL_TRUE` on success, `GL_FALSE` on error.
	 */
	GLint status;

	/**
	 * @brief The information log.
	 */
	GLchar *infoLog;

} ShaderDescriptor;

/**
 * @brief Creates a ShaderDescriptor with the specified type and Resource names.
 */
#define MakeShaderDescriptor(type, ...) \
	(ShaderDescriptor) { type, { __VA_ARGS__, NULL }, NULL, GL_FALSE }

/**
 * @brief Creates a `NULL`-terminated array of ShaderDescriptors.
 */
#define MakeShaderDescriptors(...) \
	{ \
		__VA_ARGS__, \
		MakeShaderDescriptor(GL_NONE, NULL, NULL, GL_FALSE) \
	}

/**
 * @brief The Shader type.
 * @extends Object
 */
struct Shader {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ShaderInterface *interface;

	/**
	 * @brief The Shader name.
	 */
	GLuint name;

	/**
	 * @brief The Shader type, e.g. `GL_VERTEX_SHADER`, `GL_FRAGMENT_SHADER`, etc..
	 */
	GLenum type;

	/**
	 * @brief The Shader source.
	 */
	GLchar *source;
};

/**
 * @brief The Shader interface.
 */
struct ShaderInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Shader::appendBytes(Shader *self, const uint8_t *bytes, size_t length)
	 * @brief Appends `length` of `bytes` to this Shader's source.
	 * @param self The Shader.
	 * @param bytes The Shader source bytes.
	 * @param length The length of bytes.
	 * @return The number of bytes appended, or -1 on error.
	 * @memberof Shader
	 */
	ssize_t (*appendBytes)(Shader *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn void Shader::appendData(Shader *self, const Data *data)
	 * @brief Appends the contents of Data to this Shader's source.
	 * @param self The Shader.
	 * @param data The Data to append.
	 * @return The number of bytes appended, or -1 on error.
	 * @memberof Shader
	 */
	ssize_t (*appendData)(Shader *self, const Data *data);

	/**
	 * @fn void Shader::appendResource(Shader *self, const Resource *resource)
	 * @brief Appends the contents of Resource to this Shader's source.
	 * @param self The Shader.
	 * @param resource The Resource to append.
	 * @return The number of bytes appended, or -1 on error.
	 * @memberof Shader
	 */
	ssize_t (*appendResource)(Shader *self, const Resource *resource);

	/**
	 * @fn void Shader::appendResourceName(Shader *self, const char *name)
	 * @brief Appends the contents of the named Resource to this Shader's source.
	 * @param self The Shader.
	 * @param name The name of the Resource to append.
	 * @return The number of bytes appended, or -1 on error.
	 * @memberof Shader
	 */
	ssize_t (*appendResourceName)(Shader *self, const char *name);

	/**
	 * @fn void Shader::appendSource(Shader *self, const GLchar *source)
	 * @brief Appends the give Shader source to this Shader's source.
	 * @param self The Shader.
	 * @param source The Shader source to append.
	 * @return The number of bytes appended, or -1 on error.
	 * @memberof Shader
	 */
	ssize_t (*appendSource)(Shader *self, const GLchar *source);

	/**
	 * @fn GLint Shader::compile(Shader *self)
	 * @brief Compiles this Shader.
	 * @param self The Shader.
	 * @return `GL_TRUE` on success, `GL_FALSE` on error.
	 * @memberof Shader
	 */
	GLint (*compile)(const Shader *self);

	/**
	 * @fn GLchar *Shader::infoLog(const Shader *self)
	 * @param self The Shader.
	 * @return The information log for the Shader.
	 * @memberof Shader
	 */
	GLchar *(*infoLog)(const Shader *self);

	/**
	 * @fn Shader *Shader::initWithBytes(Shader *self, GLenum type, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Shader with the given type, source bytes and length.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @param bytes The Shader source bytes.
	 * @param length The length of bytes.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithBytes)(Shader *self, GLenum type, const uint8_t *bytes, size_t length);

	/**
	 * @fn Shader *Shader::initWithData(Shader *self, GLenum type, const Data *data)
	 * @brief Initializes this Shader with the Data containing Shader source.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @param data The Data containing Shader source.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithData)(Shader *self, GLenum type, const Data *data);

	/**
	 * @fn Shader *Shader::initWithDescriptor(Shader *self, ShaderDescriptor *descriptor)
	 * @brief Initializes this Shader with the given ShaderDescriptor containing Shader source.
	 * @param self The Shader.
	 * @param descriptor The ShaderDescriptor.
	 * @return The initialized Shader, or `NULL` on error.
	 * @remarks The Shader is also compiled, and its compilation status written to `descriptor`.
	 * @memberof Shader
	 */
	Shader *(*initWithDescriptor)(Shader *self, ShaderDescriptor *descriptor);

	/**
	 * @fn Shader *Shader::initWithResource(Shader *self, GLenum type, const Resource *resource)
	 * @brief Initializes this Shader with the Resource containing Shader source.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @param resource The Resource containing Shader source.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithResource)(Shader *self, GLenum type, const Resource *resource);

	/**
	 * @fn Shader *Shader::initWithResourceName(Shader *self, GLenum type, const char *name)
	 * @brief Initializes this Shader with the named Resource containing Shader source.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @param name The name of the Resource containing Shader source.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithResourceName)(Shader *self, GLenum type, const char *name);

	/**
	 * @fn Shader *Shader::initWithSource(Shader *self, GLenum type, const GLchar *source)
	 * @brief Initializes this Shader with the given Shader source.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @param source The `NULL`-terminated Shader source.
	 * @return The initialized Shader, or `NULL` on error.
	 * @memberof Shader
	 */
	Shader *(*initWithSource)(Shader *self, GLenum type, const GLchar *source);

	/**
	 * @fn Shader *Shader::initWithType(Shader *self, GLenum type)
	 * @brief Initializes this Shader with the given type.
	 * @param self The Shader.
	 * @param type The Shader type.
	 * @return The initialized Shader, or `NULL` on error.
	 * @remarks Designated initializer.
	 * @memberof Shader
	 */
	Shader *(*initWithType)(Shader *self, GLenum type);
};

/**
 * @fn Class *Shader::_Shader(void)
 * @brief The Shader archetype.
 * @return The Shader Class.
 * @memberof Shader
 */
OBJECTIVELYGL_EXPORT Class *_Shader(void);

/**
 * @brief Frees ShaderDescriptors when they are no longer needed.
 * @param descriptors The ShaderDescriptors.
 * @relates ShaderDescriptor
 */
OBJECTIVELYGL_EXPORT void FreeShaderDescriptors(ShaderDescriptor *descriptors);
