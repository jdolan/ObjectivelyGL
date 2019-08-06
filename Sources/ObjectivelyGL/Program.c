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

#include <Objectively/MutableArray.h>

#include "Program.h"

#define _Class _Program

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Program *this = (Program *) self;

	glDeleteProgram(this->name);

	release(this->shaders);

	super(Object, self, dealloc);
}

#pragma mark - Program

/**
 * @fn Variable *Program::activeAttributes(const Program *self)
 * @memberof Program
 */
static Variable *activeAttributes(const Program *self) {

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
 * @fn Variable *Program::activeUniforms(const Program *self)
 * @memberof Program
 */
static Variable *activeUniforms(const Program *self) {

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
			$(self->shaders, addObject, shader);
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

			$(self->shaders, addObject, shader->shader);
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
 * @brief ArrayEnumerator for attaching Shaders.
 */
static void attach(const Array *array, ident obj, ident data) {
	glAttachShader(((Program *) data)->name, ((Shader *) obj)->name);
}

/**
 * @brief ArrayEnumerator for detaching Shaders.
 */
static void detach(const Array *array, ident obj, ident data) {
	glDetachShader(((Program *) data)->name, ((Shader *) obj)->name);
}

/**
 * @fn GLint Program::link(const Program *self)
 * @memberof Program
 */
static GLint link(const Program *self) {

	$((Array *) self->shaders, enumerateObjects, attach, (ident) self);

	glLinkProgram(self->name);

	$((Array *) self->shaders, enumerateObjects, detach, (ident) self);

	GLint status;
	glGetProgramiv(self->name, GL_LINK_STATUS, &status);

	return status;
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

	((ProgramInterface *) clazz->interface)->activeAttributes = activeAttributes;
	((ProgramInterface *) clazz->interface)->activeUniforms = activeUniforms;
	((ProgramInterface *) clazz->interface)->infoLog = infoLog;
	((ProgramInterface *) clazz->interface)->init = init;
	((ProgramInterface *) clazz->interface)->initWithShaders = initWithShaders;
	((ProgramInterface *) clazz->interface)->initWithDescriptor = initWithDescriptor;
	((ProgramInterface *) clazz->interface)->link = link;
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

