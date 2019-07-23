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

#include "Shader.h"

#define _Class _Shader

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Shader *this = (Shader *) self;

	glDeleteShader(this->name);

	free(this->source);
	free(this->info);

	super(Object, self, dealloc);
}

#pragma mark - Shader

/**
 * @fn ssize_t Shader::appendBytes(Shader *self, const uint8_t *bytes, size_t length)
 * @memberof Shader
 */
static ssize_t appendBytes(Shader *self, const uint8_t *bytes, size_t length) {

	if (length) {
		
		GLchar *source = realloc(self->source, strlen(self->source) + length + 1);
		if (source) {
			self->source = source;
		} else {
			return -1;
		}

		strncat(self->source, (char *) bytes, length);
	}

	return length;
}

/**
 * @fn ssize_t Shader::appendData(Shader *self, const Data *data)
 * @memberof Shader
 */
static ssize_t appendData(Shader *self, const Data *data) {
	return $(self, appendBytes, data->bytes, data->length);
}

/**
 * @fn ssize_t Shader::appendResource(Shader *self, const Resource *resource)
 * @memberof Shader
 */
static ssize_t appendResource(Shader *self, const Resource *resource) {
	return $(self, appendData, resource->data);
}

/**
 * @fn ssize_t Shader::appendResourceName(Shader *self, const char *name)
 * @memberof Shader
 */
static ssize_t appendResourceName(Shader *self, const char *name) {

	ssize_t length;

	Resource *resource = $$(Resource, resourceWithName, name);
	if (resource) {
		length = $(self, appendResource, resource);
	} else {
		length = -1;
	}
	release(resource);

	return length;
}

/**
 * @fn ssize_t Shader::appendSource(Shader *self, const GLchar *source)
 * @memberof Shader
 */
static ssize_t appendSource(Shader *self, const GLchar *source) {
	return $(self, appendBytes, (const uint8_t *) source, strlen(source));
}

/**
 * @fn GLint Shader::compile(Shader *self, GLChar **out)
 * @memberof Shader
 */
static GLint compile(Shader *self) {

	glShaderSource(self->name, 1, (const GLchar **) &self->source, NULL);

	glCompileShader(self->name);

	GLint status;
	glGetShaderiv(self->name, GL_COMPILE_STATUS, &status);

	GLint length;
	glGetShaderiv(self->name, GL_INFO_LOG_LENGTH, &length);

	self->info = malloc(length);
	assert(self->info);

	glGetShaderInfoLog(self->name, length, NULL, self->info);

	return status;
}

/**
 * @fn Shader *Shader::initWithBytes(Shader *self, GLenum type, const uint8_t *bytes, size_t length)
 * @memberof Shader
 */
static Shader *initWithBytes(Shader *self, GLenum type, const uint8_t *bytes, size_t length) {

	self = $(self, initWithType, type);
	if (self) {
		if ($(self, appendBytes, bytes, length) == -1) {
			self = release(self);
		}
	}
	return self;
}

/**
 * @fn Shader *Shader::initWithData(Shader *self, GLenum type, const Data *data)
 * @memberof Shader
 */
static Shader *initWithData(Shader *self, GLenum type, const Data *data) {

	self = $(self, initWithType, type);
	if (self) {
		if ($(self, appendData, data) == -1) {
			self = release(self);
		}
	}
	return self;
}

/**
 * @fn Shader *Shader::initWithDescriptor(Shader *self, ShaderDescriptor *descriptor)
 * @memberof Shader
 */
static Shader *initWithDescriptor(Shader *self, ShaderDescriptor *descriptor) {

	self = $(self, initWithType, descriptor->type);
	if (self) {

		for (const char **resource = descriptor->resources; *resource; resource++) {
			if ($(self, appendResourceName, *resource) == -1) {
				return release(self);
			}
		}

		descriptor->shader = self;
		descriptor->status = $(self, compile);
	}

	return self;
}

/**
 * @fn Shader *Shader::initWithResource(Shader *self, GLenum type, const Resource *resource)
 * @memberof Shader
 */
static Shader *initWithResource(Shader *self, GLenum type, const Resource *resource) {

	self = $(self, initWithType, type);
	if (self) {
		if ($(self, appendResource, resource) == -1) {
			self = release(self);
		}
	}
	return self;
}

/**
 * @fn Shader *Shader::initWithResourceName(Shader *self, GLenum type, const char *name)
 * @memberof Shader
 */
static Shader *initWithResourceName(Shader *self, GLenum type, const char *name) {

	self = $(self, initWithType, type);
	if (self) {
		if ($(self, appendResourceName, name) == -1) {
			self = release(self);
		}
	}
	return self;
}

/**
 * @fn Shader *Shader::initWithSource(Shader *self, GLenum type, const GLchar *source)
 * @memberof Shader
 */
static Shader *initWithSource(Shader *self, GLenum type, const GLchar *source) {

	self = $(self, initWithType, type);
	if (self) {
		$(self, appendSource, source);
	}
	return self;
}

/**
 * @fn Shader *Shader::initWithType(Shader *self, GLenum type)
 * @memberof Shader
 */
static Shader *initWithType(Shader *self, GLenum type) {

	self = (Shader *) super(Object, self, init);
	if (self) {
		self->name = glCreateShader(type);
		if (self->name) {
			self->type = type;
			self->source = strdup("");
			assert(self->source);
		} else {
			self = release(self);
		}
	}

	return self;
}


#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((ShaderInterface *) clazz->interface)->compile = compile;
	((ShaderInterface *) clazz->interface)->appendBytes = appendBytes;
	((ShaderInterface *) clazz->interface)->appendData = appendData;
	((ShaderInterface *) clazz->interface)->appendResource = appendResource;
	((ShaderInterface *) clazz->interface)->appendResourceName = appendResourceName;
	((ShaderInterface *) clazz->interface)->appendSource = appendSource;
	((ShaderInterface *) clazz->interface)->initWithBytes = initWithBytes;
	((ShaderInterface *) clazz->interface)->initWithData = initWithData;
	((ShaderInterface *) clazz->interface)->initWithDescriptor = initWithDescriptor;
	((ShaderInterface *) clazz->interface)->initWithResource = initWithResource;
	((ShaderInterface *) clazz->interface)->initWithResourceName = initWithResourceName;
	((ShaderInterface *) clazz->interface)->initWithSource = initWithSource;
	((ShaderInterface *) clazz->interface)->initWithType = initWithType;
}

/**
 * @fn Class *Shader::_Shader(void)
 * @memberof Shader
 */
Class *_Shader(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Shader",
			.superclass = _Object(),
			.instanceSize = sizeof(Shader),
			.interfaceOffset = offsetof(Shader, interface),
			.interfaceSize = sizeof(ShaderInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
