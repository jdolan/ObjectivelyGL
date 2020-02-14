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

#include "Texture.h"

#define _Class _Texture

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Texture *this = (Texture *) self;

	glDeleteTextures(1, &this->name);

	super(Object, self, dealloc);
}

#pragma mark - Texture

/**
 * @fn void Texture::bind(const Texture *self, GLenum target)
 * @memberof Texture
 */
static void bind(const Texture *self, GLenum target) {
	glBindTexture(target, self->name);
}

/**
 * @fn Texture *Texture::init(Texture *self)
 * @memberof Texture
 */
static Texture *init(Texture *self) {

	self = (Texture *) super(Object, self, init);
	if (self) {
		glGenTextures(1, &self->name);
		if (self->name) {

		} else {
			self = release(self);
		}
	}

	return self;
}

/**
 * @fn Texture *Texture::initWithTextureData(Texture *self, const TextureData *data)
 * @memberof Texture
 */
static Texture *initWithTextureData(Texture *self, const TextureData *data) {

	self = $(self, init);
	if (self) {
		$(self, bind, data->target);
		$(self, writeData, data);
	}
	return self;
}

/**
 * @fn void Texture::unbind(const Texture *self, GLenum target)
 * @memberof Texture
 */
static void unbind(const Texture *self, GLenum target) {
	glBindTexture(target, 0);
}

/**
 * @fn void Texture::writeData(const Texture *self, const TextureData *data)
 * @memberof Texture
 */
static void writeData(Texture *self, const TextureData *data) {

	switch (data->target) {
		case GL_TEXTURE_1D:
			glTexImage1D(data->target,
						 data->level,
						 data->internalFormat,
						 data->width,
						 0,
						 data->format,
						 data->type,
						 data->data);
			break;
		case GL_TEXTURE_1D_ARRAY:
		case GL_TEXTURE_2D:
			glTexImage2D(data->target,
						 data->level,
						 data->internalFormat,
						 data->width,
						 data->height,
						 0,
						 data->format,
						 data->type,
						 data->data);
			break;
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_3D:
			glTexImage3D(data->target,
						 data->level,
						 data->internalFormat,
						 data->width,
						 data->height,
						 data->depth,
						 0,
						 data->format,
						 data->type,
						 data->data);
			break;
		default:
			assert(0);
			break;
	}
}

/**
 * @fn void Texture::writeSubData(const Texture *self, const TextureSubData *data)
 * @memberof Texture
 */
static void writeSubData(Texture *self, const TextureSubData *data) {

	switch (data->target) {
		case GL_TEXTURE_1D:
			glTexSubImage1D(data->target,
							data->level,
							data->xOffset,
							data->width,
							data->format,
							data->type,
							data->data);
			break;
		case GL_TEXTURE_1D_ARRAY:
		case GL_TEXTURE_2D:
			glTexSubImage2D(data->target,
							data->level,
							data->xOffset,
							data->yOffset,
							data->width,
							data->height,
							data->format,
							data->type,
							data->data);
			break;
		case GL_TEXTURE_2D_ARRAY:
		case GL_TEXTURE_3D:
			glTexSubImage3D(data->target,
							data->level,
							data->xOffset,
							data->yOffset,
							data->zOffset,
							data->width,
							data->height,
							data->depth,
							data->format,
							data->type,
							data->data);
			break;
		default:
			assert(0);
			break;
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((TextureInterface *) clazz->interface)->bind = bind;
	((TextureInterface *) clazz->interface)->init = init;
	((TextureInterface *) clazz->interface)->initWithTextureData = initWithTextureData;
	((TextureInterface *) clazz->interface)->unbind = unbind;
	((TextureInterface *) clazz->interface)->writeData = writeData;
	((TextureInterface *) clazz->interface)->writeSubData = writeSubData;
}

/**
 * @fn Class *Texture::_Texture(void)
 * @memberof Texture
 */
Class *_Texture(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Texture",
			.superclass = _Object(),
			.instanceSize = sizeof(Texture),
			.interfaceOffset = offsetof(Texture, interface),
			.interfaceSize = sizeof(TextureInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
