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

#include <Objectively/Object.h>

#include <ObjectivelyGL/Types.h>

/**
 * @file
 * @brief ..
 */

typedef struct Texture Texture;
typedef struct TextureInterface TextureInterface;

/**
 * @brief TextureData is used to initialize or modify a Texture.
 * @see glTexImage1D, glTexImage2D, glTexImage3D
 */
typedef struct TextureData {

	/**
	 * @brief The target, e.g. `GL_TEXTURE_2D`, `GL_TEXTURE_3D`, etc.
	 */
	GLenum target;

	/**
	 * @brief The mipmap level, default is `0`.
	 */
	GLint level;

	/**
	 * @brief The internal (input) format, e.g. `GL_RGB`.
	 */
	GLint internalFormat;

	/**
	 * @brief The width, height and depth of the image data.
	 */
	GLsizei width, height, depth;

	/**
	 * @brief The desired storage format, e.g. `GL_RGB`.
	 */
	GLenum format;

	/**
	 * @brief The desired data type, e.g. `GL_UNSIGNED_BYTE`.
	 */
	GLenum type;

	/**
	 * @brief The image data.
	 */
	const ident data;
} TextureData;

#define MakeTextureData(target, level, internalFormat, width, height, depth, format, type, data) \
	(TextureData) { (target), (level), (internalFormat), (width), (height), (format), (type), (data) }

#define MakeTextureData2D_RGB(width, height, data) \
	MakeTextureData(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data)

#define MakeTextureData2D_RGBA(width, height, data) \
	MakeTextureData(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data)

#define MakeTextureData3D_RGB(width, height, depth, data) \
	MakeTextureData(GL_TEXTURE_3D, 0, GL_RGB, width, height, depth, GL_RGB, GL_UNSIGNED_BYTE, data)

#define MakeTextureData3D_RGBA(width, height, depth, data) \
	MakeTextureData(GL_TEXTURE_3D, 0, GL_RGBA, width, height, depth, GL_RGBA, GL_UNSIGNED_BYTE, data)

/**
 * @brief TextureSubData is used to modify part of a Texture.
 * @see glTexSubImage1D, glTexSubImage2D, glTexSubImage3D
 */
typedef struct TextureSubData {

	/**
	 * @brief The target, e.g. `GL_TEXTURE_2D`, `GL_TEXTURE_3D`, etc.
	 */
	GLenum target;

	/**
	 * @brief The mipmap level, default is `0`.
	 */
	GLint level;

	/**
	 * @brief The texel offsets of the subimage data.
	 */
	GLint xOffset, yOffset, zOffset;

	/**
	 * @brief The width, height and depth of the subimage data.
	 */
	GLsizei width, height, depth;

	/**
	 * @brief The desired storage format, e.g. `GL_RGB`.
	 */
	GLenum format;

	/**
	 * @brief The desired data type, e.g. `GL_UNSIGNED_BYTE`.
	 */
	GLenum type;

	/**
	 * @brief The subimage data.
	 */
	const ident data;
} TextureSubData;

/**
 * @brief The Texture type.
 * @extends Object
 */
struct Texture {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	TextureInterface *interface;

	/**
	 * @brief The name.
	 */
	GLuint name;
};

/**
 * @brief The Texture interface.
 */
struct TextureInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Texture::bind(const Texture *self, GLenum target)
	 * @brief Binds this texture to the specified target.
	 * @param self The Texture.
	 * @param target The target, e.g. `GL_TEXTURE_2D`, `GL_TEXTURE_ARRAY_2D`, etc.
	 * @memberof Texture
	 */
	void (*bind)(const Texture *self, GLenum target);

	/**
	 * @fn Texture *Texture::init(Texture *self)
	 * @brief Initializes this Texture.
	 * @param self The Texture.
	 * @return The initialized Texture, or `NULL` on error.
	 * @memberof Texture
	 */
	Texture *(*init)(Texture *self);

	/**
	 * @fn Texture *Texture::initWithTextureData(Texture *self, const TextureData *data)
	 * @brief Initializes this Texture with the specified data.
	 * @param self The Texture.
	 * @param data The TextureData.
	 * @return The initialized Texture, or `NULL` on error.
	 * @memberof Texture
	 */
	Texture *(*initWithTextureData)(Texture *self, const TextureData *data);

	/**
	 * @fn void Texture::unbind(const Texture *self, GLenum target)
	 * @brief Unbinds this Texture from the specified target.
	 * @param self The Texture.
	 * @param target The target, e.g. `GL_TEXTURE_2D`, `GL_TEXTURE_3D`, etc.
	 * @memberof Texture
	 */
	void (*unbind)(const Texture *self, GLenum target);

	/**
	 * @fn void Texture::writeData(const Texture *self, const TextureData *data)
	 * @brief Writes image data to this Texture.
	 * @param self The Texture.
	 * @param data The TextureData.
	 * @memberof Texture
	 */
	void (*writeData)(Texture *self, const TextureData *data);

	/**
	 * @fn void Texture::writeSubData(const Texture *self, const TextureSubData *data)
	 * @brief Writes subimage data to this Texture.
	 * @param self The Texture.
	 * @param data The TextureSubData.
	 * @memberof Texture
	 */
	void (*writeSubData)(Texture *self, const TextureSubData *data);
};

/**
 * @fn Class *Texture::_Texture(void)
 * @brief The Texture archetype.
 * @return The Texture Class.
 * @memberof Texture
 */
OBJECTIVELY_EXPORT Class *_Texture(void);
