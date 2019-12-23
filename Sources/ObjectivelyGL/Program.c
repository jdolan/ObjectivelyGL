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
#include <stdarg.h>
#include <stdlib.h>

#include "Program.h"

#define _Class _Program

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Program *this = (Program *) self;

	$(this, detachAll);

	release(this->shaders);

	glDeleteProgram(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Program

/**
 * @fn Variable Program::attributeForLocation(const Program *self, const GLuint index)
 * @memberof Program
 */
static Variable attributeForLocation(const Program *self, const GLuint index) {

	Variable u = { index };
	glGetActiveUniform(self->name, index, sizeof(u.name) - 1, NULL, &u.size, &u.type, u.name);
	return u;
}

/**
 * @fn Variable Program::attributeForName(const Program *self, const GLchar *name)
 * @memberof Program
 */
static Variable attributeForName(const Program *self, const GLchar *name) {

	const GLint location = $(self, attributeLocation, name);
	if (location == -1) {
		return (Variable) {
			.index = -1,
			.type = GL_NONE
		};
	}

	return $(self, attributeForLocation, location);
}

/**
 * @fn GLint Program::attributeLocation(const Program *self, const GLchar *name)
 * @memberof Program
 */
static GLint attributeLocation(const Program *self, const GLchar *name) {
	return glGetAttribLocation(self->name, name);
}

/**
 * @fn Variable *Program::attributes(const Program *self)
 * @memberof Program
 */
static Variable *attributes(const Program *self) {

	GLuint count;
	glGetProgramiv(self->name, GL_ACTIVE_ATTRIBUTES, (GLint *) &count);

	Variable *attributes = calloc(count + 1, sizeof(Variable));
	assert(attributes);

	Variable *a = attributes;
	for (GLuint i = 0; i < count; a->index = i, i++, a++) {
		glGetActiveAttrib(self->name, i, sizeof(a->name) - 1, NULL, &a->size, &a->type, a->name);
	}

	return attributes;
}

/**
 * @fn void Program::attach(Program *self, Shader *shader)
 * @memberof Program
 */
static void attach(Program *self, Shader *shader) {

	glAttachShader(self->name, shader->name);

	$(self->shaders, addObject, shader);
}

/**
 * @fn void Program::detach(Program *self, Shader *shader)
 * @memberof Program
 */
static void detach(Program *self, Shader *shader) {

	glDetachShader(self->name, shader->name);

	$(self->shaders, removeObject, shader);
}

/**
 * @brief ArrayEnumerator for detachAll.
 */
static void detachAll_enumerator(const Array *array, ident obj, ident data) {
	glDetachShader(((Program *) data)->name, ((Shader *) obj)->name);
}

/**
 * @fn void Program::detachAll(Program *self)
 * @memberof Program
 */
static void detachAll(Program *self) {
	$(self->shaders, removeAllObjectsWithEnumerator, detachAll_enumerator, self);
}

/**
 * @fn GLchar *Program::infoLog(const Program *self)
 * @memberof Program
 */
static GLchar *infoLog(const Program *self) {

	GLint length;
	glGetProgramiv(self->name, GL_INFO_LOG_LENGTH, &length);

	GLchar *info = calloc(length, 1);
	assert(info);

	glGetProgramInfoLog(self->name, length, NULL, info);
	return info;
}

/**
 * @fn Program *Program::init(Program *self)
 * @memberof Program
 */
static Program *init(Program *self) {

	self = (Program *) super(Object, self, init);
	if (self) {

		self->name = glCreateProgram();
		if (!self->name) {
			return release(self);
		}

		self->shaders = $$(MutableArray, array);
	}

	return self;
}

/**
 * @fn Program *Program::initWithShaders(Program *self, ...)
 * @memberof Program
 */
static Program *initWithShaders(Program *self, ...) {

	self = $(self, init);
	if (self) {

		va_list args;
		va_start(args, self);

		Shader *shader;
		while ((shader = va_arg(args, Shader *))) {
			$(self, attach, shader);
		}

		va_end(args);
	}

	return self;
}

/**
 * @fn Program *Program::initWithDescriptors(Program *self, ProgramDescriptor *descriptor)
 * @memberof Program
 */
static Program *initWithDescriptor(Program *self, ProgramDescriptor *descriptor) {

	self = $(self, init);
	if (self) {

		for (ShaderDescriptor *shader = descriptor->shaders;
			 shader->type != GL_NONE;
			 shader++) {

			$(alloc(Shader), initWithDescriptor, shader);
		}

		for (ShaderDescriptor *shader = descriptor->shaders;
	 		 shader->type != GL_NONE;
			 shader++) {

			if (shader->shader == NULL) {
				return release(self);
			}

			$(self, attach, shader->shader);
		}

		descriptor->status = $(self, link);
		descriptor->infoLog = $(self, infoLog);

		if (descriptor->status == GL_TRUE) {
			descriptor->program = self;
		} else {
			return release(self);
		}
	}

	return self;
}

/**
 * @fn GLint Program::link(const Program *self)
 * @memberof Program
 */
static GLint link(const Program *self) {

	glLinkProgram(self->name);

	GLint status;
	glGetProgramiv(self->name, GL_LINK_STATUS, &status);

	return status;
}

/**
 * @fn void Program::setUniform(const Program *self, const Variable *variable, const GLvoid *value)
 * @memberof Program
 */
static void setUniform(const Program *self, const Variable *variable, const GLvoid *value) {

	switch (variable->type) {
		case GL_FLOAT:
			switch (variable->size) {
				case 1:
					glUniform1f(variable->index, *(GLfloat *) value);
					break;
				case 2:
					glUniform2fv(variable->index, 2, (const GLfloat *) value);
					break;
				case 3:
					glUniform3fv(variable->index, 3, (const GLfloat *) value);
					break;
				case 4:
					glUniform4fv(variable->index, 4, (const GLfloat *) value);
					break;
			}
			break;

		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(variable->index, 1, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(variable->index, 1, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_INT:
			switch (variable->size) {
				case 1:
					glUniform1i(variable->index, *(GLint *) value);
					break;
				case 2:
					glUniform2iv(variable->index, 2, (const GLint *) value);
					break;
				case 3:
					glUniform3iv(variable->index, 3, (const GLint *) value);
					break;
				case 4:
					glUniform4iv(variable->index, 4, (const GLint *) value);
					break;
			}
			break;

		case GL_UNSIGNED_INT:
			switch (variable->size) {
				case 1:
					glUniform1ui(variable->index, *(const GLint *) value);
					break;
				case 2:
					glUniform2uiv(variable->index, 2, (const GLuint *) value);
					break;
				case 3:
					glUniform3uiv(variable->index, 3, (const GLuint *) value);
					break;
				case 4:
					glUniform4uiv(variable->index, 4, (const GLuint *) value);
					break;
			}
			break;

		default:
			// TODO: Print something?
			break;
	}
}

/**
 * @fn void Program::setUniformForName(const Program *self, const GLchar *name, const void *value)
 * @memberof Program
*/
static void setUniformForName(const Program *self, const GLchar *name, const void *value) {

	const Variable variable = $(self, uniformForName, name);
	$(self, setUniform, &variable, value);
}

/**
 * @fn GLint Program::uniformBlockLocation(const Program *self, const GLchar *name)
 * @memberof Program
 */
static GLint uniformBlockLocation(const Program *self, const GLchar *name) {
	return glGetUniformBlockIndex(self->name, name);
}

/**
 * @fn void Program::uniformBlockBinding(const Program *self, GLuint block, GLuint index)
 * @memberof Program
 */
static void uniformBlockBinding(const Program *self, GLuint block, GLuint index) {
	glUniformBlockBinding(self->name, block, index);
}

/**
 * @fn Variable Program::uniformForLocation(const Program *self, const GLuint index)
 * @memberof Program
 */
static Variable uniformForLocation(const Program *self, const GLuint index) {

	Variable u = { index };
	glGetActiveUniform(self->name, index, sizeof(u.name) - 1, NULL, &u.size, &u.type, u.name);
	return u;
}

/**
 * @fn Variable Program::uniformForName(const Program *self, const GLchar *name)
 * @memberof Program
 */
static Variable uniformForName(const Program *self, const GLchar *name) {

	const GLint index = $(self, uniformLocation, name);
	if (index == -1) {
		return (Variable) {
			.index = -1,
			.type = GL_NONE
		};
	}

	return $(self, uniformForLocation, index);
}

/**
 * @fn GLint Program::uniformLocation(const Program *self, const GLchar *name)
 * @memberof Program
 */
static GLint uniformLocation(const Program *self, const GLchar *name) {
	return glGetUniformLocation(self->name, name);
}

/**
 * @fn Variable *Program::uniforms(const Program *self)
 * @memberof Program
 */
static Variable *uniforms(const Program *self) {

	GLuint count;
	glGetProgramiv(self->name, GL_ACTIVE_UNIFORMS, (GLint *) &count);

	Variable *uniforms = calloc(count + 1, sizeof(Variable));
	assert(uniforms);

	Variable *u = uniforms;
	for (GLuint i = 0; i < count; u->index = i, i++, u++) {
		glGetActiveUniform(self->name, i, sizeof(u->name) - 1, NULL, &u->size, &u->type, u->name);
	}

	return uniforms;
}

/**
 * @fn void Program::use(const Program *self)
 * @memberof Program
 */
static void use(const Program *self) {
	glUseProgram(self->name);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ProgramInterface *) clazz->interface)->attributeForLocation = attributeForLocation;
	((ProgramInterface *) clazz->interface)->attributeForName = attributeForName;
	((ProgramInterface *) clazz->interface)->attributeLocation = attributeLocation;
	((ProgramInterface *) clazz->interface)->attributes = attributes;
	((ProgramInterface *) clazz->interface)->attach = attach;
	((ProgramInterface *) clazz->interface)->detach = detach;
	((ProgramInterface *) clazz->interface)->detachAll = detachAll;
	((ProgramInterface *) clazz->interface)->infoLog = infoLog;
	((ProgramInterface *) clazz->interface)->init = init;
	((ProgramInterface *) clazz->interface)->initWithShaders = initWithShaders;
	((ProgramInterface *) clazz->interface)->initWithDescriptor = initWithDescriptor;
	((ProgramInterface *) clazz->interface)->link = link;
	((ProgramInterface *) clazz->interface)->uniformBlockLocation = uniformBlockLocation;
	((ProgramInterface *) clazz->interface)->uniformBlockBinding = uniformBlockBinding;
	((ProgramInterface *) clazz->interface)->uniformForLocation = uniformForLocation;
	((ProgramInterface *) clazz->interface)->uniformForName = uniformForName;
	((ProgramInterface *) clazz->interface)->uniformLocation = uniformLocation;
	((ProgramInterface *) clazz->interface)->uniforms = uniforms;
	((ProgramInterface *) clazz->interface)->setUniform = setUniform;
	((ProgramInterface *) clazz->interface)->setUniformForName = setUniformForName;
	((ProgramInterface *) clazz->interface)->use = use;
}

/**
 * @fn Class *Program::_Program(void)
 * @memberof Program
 */
Class *_Program(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Program",
			.superclass = _Object(),
			.instanceSize = sizeof(Program),
			.interfaceOffset = offsetof(Program, interface),
			.interfaceSize = sizeof(ProgramInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class

void FreeProgramDescriptor(ProgramDescriptor *descriptor) {

	for (ShaderDescriptor *shader = descriptor->shaders;
		 shader->type != GL_NONE;
		 shader++) {

		FreeShaderDescriptor(shader);
	}

	free(descriptor->infoLog);
}

