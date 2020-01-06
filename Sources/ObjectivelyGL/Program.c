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

#include <Objectively/MutableString.h>

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
	release(this->uniformBlocks);

	glDeleteProgram(this->name);

	super(Object, self, dealloc);
}

/**
 * @see Object::description(const Object *)
 */
static String *description(const Object *self) {

	Program *this = (Program *) self;

	MutableString *desc = $(super(Object, self, description), mutableCopy);
	$(desc, appendFormat, " %d\n", this->name);

	$(desc, appendFormat, "\n");
	$(desc, appendFormat, "%-32s %s %s %s %s\n", "Attribute", "Index", "Location", "Size", "Type");
	const Variable *attr = this->attributes->elements;
	for (size_t i = 0; i < this->attributes->count; i++, attr++) {
		$(desc, appendFormat, "%-32s %-5d %-8d %-4d %-4d\n",
				attr->name,
				attr->index,
				attr->location,
				attr->size,
				attr->type);
	}

	$(desc, appendFormat, "\n");
	$(desc, appendFormat, "%-32s %s %s %s %s\n", "Uniform", "Index", "Location", "Size", "Type");
	const Variable *uniform = this->uniforms->elements;
	for (size_t i = 0; i < this->uniforms->count; i++, uniform++) {
		$(desc, appendFormat, "%-32s %-5d %-8d %-4d %-4d\n",
				uniform->name,
				uniform->index,
				uniform->location,
				uniform->size,
				uniform->type);
	}

	$(desc, appendFormat, "\n");
	$(desc, appendFormat, "%-32s %s %s\n", "Uniform Block", "Index", "Binding");
	const UniformBlock *block = this->uniformBlocks->elements;
	for (size_t i = 0; i < this->uniformBlocks->count; i++, block++) {
		$(desc, appendFormat, "%-32s %5d %7d\n",
				block->name,
				block->index,
				block->binding);
	}

	return (String *) desc;
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

		self->uniformBlocks = $$(Vector, vectorWithSize, sizeof(UniformBlock));
		assert(self->uniformBlocks);
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

		GLint activeAttributes;
		glGetProgramiv(self->name, GL_ACTIVE_ATTRIBUTES, &activeAttributes);
		for (GLint i = 0; i < activeAttributes; i++) {
			Variable var = { .index = i };
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

		GLint activeUniforms;
		glGetProgramiv(self->name, GL_ACTIVE_UNIFORMS, &activeUniforms);
		for (GLint i = 0; i < activeUniforms; i++) {
			Variable var = { .index = i };
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

		GLint activeUniformBlocks;
		glGetProgramiv(self->name, GL_ACTIVE_UNIFORM_BLOCKS, &activeUniformBlocks);
		for (GLint i = 0; i < activeUniformBlocks; i++) {
			UniformBlock block = { .index = i };
			glGetActiveUniformBlockName(self->name,
										i,
										sizeof(block.name) - 1,
										NULL,
										block.name);

			glGetActiveUniformBlockiv(self->name,
									  i,
									  GL_UNIFORM_BLOCK_BINDING,
									  &block.binding);

			$(self->uniformBlocks, addElement, &block);
		}
	}

	return status;
}

/**
 * @fn void Program::setUniform(const Program *self, const Variable *variable, const GLvoid *value)
 * @memberof Program
 */
static void setUniform(const Program *self, const Variable *var, const GLvoid *value) {

	switch (var->type) {
		case GL_FLOAT:
			glUniform1f(var->location, *(GLfloat *) value);
			break;

		case GL_FLOAT_MAT3:
			glUniformMatrix3fv(var->location, var->size, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_FLOAT_MAT4:
			glUniformMatrix4fv(var->location, var->size, GL_FALSE, (const GLfloat *) value);
			break;

		case GL_FLOAT_VEC2:
			glUniform2fv(var->location, var->size, (const GLfloat *) value);
			break;

		case GL_FLOAT_VEC3:
			glUniform3fv(var->location, var->size, (const GLfloat *) value);
			break;

		case GL_FLOAT_VEC4:
			glUniform4fv(var->location, var->size, (const GLfloat *) value);
			break;

		case GL_INT:
			glUniform1i(var->location, *(GLint *) value);
			break;
		case GL_INT_VEC2:
			glUniform2iv(var->location, var->size, (const GLint *) value);
			break;
		case GL_INT_VEC3:
			glUniform3iv(var->location, var->size, (const GLint *) value);
			break;
		case GL_INT_VEC4:
			glUniform4iv(var->location, var->size, (const GLint *) value);
			break;

		case GL_UNSIGNED_INT:
			glUniform1ui(var->location, *(const GLint *) value);
			break;
		case GL_UNSIGNED_INT_VEC2:
			glUniform2uiv(var->location, var->size, (const GLuint *) value);
			break;
		case GL_UNSIGNED_INT_VEC3:
			glUniform3uiv(var->location, var->size, (const GLuint *) value);
			break;
		case GL_UNSIGNED_INT_VEC4:
			glUniform4uiv(var->location, var->size, (const GLuint *) value);
			break;

		default:
			assert(false);
			break;
	}

	assert(glGetError() == GL_NO_ERROR);
}

/**
 * @fn void Program::setUniformBlockBinding(const Program *self, const UniformBlock *block, GLint index)
 * @memberof Program
 */
static void setUniformBlockBinding(const Program *self, const UniformBlock *block, GLuint index) {

	glUniformBlockBinding(self->name, block->index, block->binding);

	((UniformBlock *) block)->binding = index;
}

/**
 * @fn void Program::setUniformBlockBindingForName(const Program *self, const UniformBlock *block, GLint index)
 * @memberof Program
 */
static void setUniformBlockBindingForName(const Program *self, const GLchar *name, GLuint index) {

	const UniformBlock *block = $(self, uniformBlockForName, name);
	if (block) {
		$(self, setUniformBlockBinding, block, index);
	} else {
		assert(false);
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
 * @fn const UniformBlock *Program::uniformBlockForName(const Program *self, const GLchar *name)
 * @memberof Program
 */
static const UniformBlock *uniformBlockForName(const Program *self, const GLchar *name) {

	UniformBlock *block = self->uniformBlocks->elements;
	for (size_t i = 0; i < self->uniformBlocks->count; i++, block++) {
		if (!strcmp(name, block->name)) {
			return block;
		}
	}

	return NULL;
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

	if (self->use) {
		self->use(self);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;
	((ObjectInterface *) clazz->interface)->description = description;

	((ProgramInterface *) clazz->interface)->attributeForName = attributeForName;
	((ProgramInterface *) clazz->interface)->attach = attach;
	((ProgramInterface *) clazz->interface)->detach = detach;
	((ProgramInterface *) clazz->interface)->detachAll = detachAll;
	((ProgramInterface *) clazz->interface)->infoLog = infoLog;
	((ProgramInterface *) clazz->interface)->init = init;
	((ProgramInterface *) clazz->interface)->initWithShaders = initWithShaders;
	((ProgramInterface *) clazz->interface)->initWithDescriptor = initWithDescriptor;
	((ProgramInterface *) clazz->interface)->link = link;
	((ProgramInterface *) clazz->interface)->setUniform = setUniform;
	((ProgramInterface *) clazz->interface)->setUniformBlockBinding = setUniformBlockBinding;
	((ProgramInterface *) clazz->interface)->setUniformBlockBindingForName = setUniformBlockBindingForName;
	((ProgramInterface *) clazz->interface)->setUniformForName = setUniformForName;
	((ProgramInterface *) clazz->interface)->uniformBlockForName = uniformBlockForName;
	((ProgramInterface *) clazz->interface)->uniformForName = uniformForName;
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

