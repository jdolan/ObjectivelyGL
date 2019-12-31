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
#include <string.h>

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
	release(this->attributes);
	release(this->uniforms);

	glDeleteProgram(this->name);

	super(Object, self, dealloc);
}

#pragma mark - Program

/**
 * @fn void Program::attach(Program *self, Shader *shader)
 * @memberof Program
 */
static void attach(Program *self, Shader *shader) {

	glAttachShader(self->name, shader->name);

	$(self->shaders, addObject, shader);
}

/**
 * @fn Variable *Program::attributeForName(const Program *self, const GLchar *name)
 * @memberof Program
 */
static const Variable *attributeForName(const Program *self, const GLchar *name) {

	const Variable *attr = self->attributes->elements;
	for (size_t i = 0; i < self->attributes->count; i++, attr++) {
		if (!strcmp(name, attr->name)) {
			return attr;
		}
	}

	return NULL;
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
		assert(self->shaders);

		self->attributes = $$(Vector, vectorWithSize, sizeof(Variable));
		assert(self->attributes);

		self->uniforms = $$(Vector, vectorWithSize, sizeof(Variable));
		assert(self->uniforms);
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
 * @fn GLint Program::link(Program *self)
 * @memberof Program
 */
static GLint link(Program *self) {

	glLinkProgram(self->name);

	GLint status;
	glGetProgramiv(self->name, GL_LINK_STATUS, &status);

	if (status == GL_TRUE) {

		GLuint activeAttributes;
		glGetProgramiv(self->name, GL_ACTIVE_ATTRIBUTES, (GLint *) &activeAttributes);
		for (GLuint i = 0; i < activeAttributes; i++) {
			Variable var;
			glGetActiveAttrib(self->name,
							  i,
							  sizeof(var.name) - 1,
							  NULL,
							  &var.size,
							  &var.type,
							  var.name);

			var.location = glGetAttribLocation(self->name, var.name);
			$(self->attributes, addElement, &var);
		}

		GLuint activeUniforms;
		glGetProgramiv(self->name, GL_ACTIVE_UNIFORMS, (GLint *) &activeUniforms);
		for (GLuint i = 0; i < activeUniforms; i++) {
			Variable var;
			glGetActiveUniform(self->name,
							   i,
							   sizeof(var.name) - 1,
							   NULL,
							   &var.size,
							   &var.type,
							   var.name);

			var.location = glGetUniformLocation(self->name, var.name);
			$(self->uniforms, addElement, &var);
		}
	}

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
					glUniform1f(variable->location, *(GLfloat *) value);
					break;
				case 2:
					glUniform2fv(variable->location, 2, (const GLfloat *) value);
					break;
				case 3:
					glUniform3fv(variable->location, 3, (const GLfloat *) value);
					break;
				case 4:
					glUniform4fv(variable->location, 4, (const GLfloat *) value);
					break;
			}
			break;

		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(variable->location, 1, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(variable->location, 1, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_INT:
			switch (variable->size) {
				case 1:
					glUniform1i(variable->location, *(GLint *) value);
					break;
				case 2:
					glUniform2iv(variable->location, 2, (const GLint *) value);
					break;
				case 3:
					glUniform3iv(variable->location, 3, (const GLint *) value);
					break;
				case 4:
					glUniform4iv(variable->location, 4, (const GLint *) value);
					break;
			}
			break;

		case GL_UNSIGNED_INT:
			switch (variable->size) {
				case 1:
					glUniform1ui(variable->location, *(const GLint *) value);
					break;
				case 2:
					glUniform2uiv(variable->location, 2, (const GLuint *) value);
					break;
				case 3:
					glUniform3uiv(variable->location, 3, (const GLuint *) value);
					break;
				case 4:
					glUniform4uiv(variable->location, 4, (const GLuint *) value);
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

	const Variable *var = $(self, uniformForName, name);
	if (var) {
		$(self, setUniform, var, value);
	} else {
		assert(false);
	}
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
 * @fn const Variable *Program::uniformForName(const Program *self, const GLchar *name)
 * @memberof Program
 */
static const Variable *uniformForName(const Program *self, const GLchar *name) {

	const Variable *var = self->uniforms->elements;
	for (size_t i = 0; i < self->uniforms->count; i++, var++) {
		if (!strcmp(name, var->name)) {
			return var;
		}
	}

	return NULL;
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

	((ProgramInterface *) clazz->interface)->attributeForName = attributeForName;
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
	((ProgramInterface *) clazz->interface)->uniformForName = uniformForName;
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

