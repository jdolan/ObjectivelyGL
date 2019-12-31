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
#include <Objectively/Vector.h>

#include <ObjectivelyGL/VertexArray.h>

/**
 * @file
 * @brief ..
 */

typedef struct Model Model;
typedef struct ModelInterface ModelInterface;

typedef struct {

	vec3s position;
	vec4ubs color;
	vec2s diffuse;
	vec2s lightmap;
	vec3s normal;
	vec3s tangent;
	vec3s bitangent;

} ModelVertex;

typedef struct {

	/**
	 * @brief The mesh name.
	 */
	char *name;

	/**
	 * @brief The primitive type for this mesh, e.g. `GL_TRIANGLES`.
	 */
	GLenum type;

	/**
	 * @brief The number of elements in this mesh.
	 */
	GLsizei count;

	/**
	 * @brief The offset of this mesh's elements in the Model's elements.
	 */
	GLsizeiptr elements;

} ModelMesh;

/**
 * @brief The Model type.
 * @extends Object
 */
struct Model {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	ModelInterface *interface;

	/**
	 * @brief The draw elements.
	 */
	Vector *elements;

	/**
	 * @brief The ModelMeshes.
	 */
	Vector *meshes;

	/**
	 * @brief The ModelVertices.
	 */
	Vector *vertices;
};

/**
 * @brief The Model interface.
 */
struct ModelInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Buffer *Model::elementsBuffer(const Model *self)
	 * @param self The Model.
	 * @return A Buffer containing this Model's elements data.
	 * @memberof Model
	 */
	Buffer *(*elementsBuffer)(const Model *self);

	/**
	 * @fn Model *Model::init(Model *self)
	 * @brief Initializes this Model.
	 * @param self The Model.
	 * @return The initialized Model, or `NULL` on error.
	 * @memberof Model
	 */
	Model *(*init)(Model *self);

	/**
	 * @fn Model *Model::initWithBytes(Model *self, const uint8_t *bytes, size_t length)
	 * @brief Initializes this Model with the specified bytes and length.
	 * @param self The Model.
	 * @param bytes The Model data bytes.
	 * @param length The length of bytes.
	 * @return The initialized Model, or `NULL` on error.
	 * @memberof Model
	 */
	Model *(*initWithBytes)(Model *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn Model *Model::initWithResource(Model *self, const Data *data)
	 * @brief Initializes this Model with the specified Data.
	 * @param self The Model.
	 * @param data The Data containing Model data.
	 * @return The initialized Model, or `NULL` on error.
	 * @memberof Model
	 */
	Model *(*initWithData)(Model *self, const Data *data);

	/**
	 * @fn Model *Model::initWithResource(Model *self, const Resource *resource)
	 * @brief Initializes this Model with the specified Resource.
	 * @param self The Model.
	 * @param resource The Resource containing Model data.
	 * @return The initialized Model, or `NULL` on error.
	 * @memberof Model
	 */
	Model *(*initWithResource)(Model *self, const Resource *resource);

	/**
	 * @fn Model *Model::initWithResourceName(Model *self, const char *name)
	 * @brief Initializes this Model with the specified Resource name.
	 * @param self The Model.
	 * @param name The name of the Resource containing Model data.
	 * @return The initialized Model, or `NULL` on error.
	 * @memberof Model
	 */
	Model *(*initWithResourceName)(Model *self, const char *name);

	/**
	 * @fn void Model::load(Model *self, const uint8_t *bytes, size_t length)
	 * @brief Loads this Model from the specified data.
	 * @param self The Model.
	 * @param bytes The model data.
	 * @param length The length of bytes.
	 * @remarks Subclasses must override this method.
	 * @memberof Model
	 */
	void (*load)(Model *self, const uint8_t *bytes, size_t length);

	/**
	 * @fn VertexArray *Model::vertexArray(const Model *self, const Attribute *attributes)
	 * @param self The Model.
	 * @param attributes The tagged Attributes.
	 * @return A VertexArray containing the tagged Attributes of this Model's vertex data.
	 * @memberof Model
	 */
	VertexArray *(*vertexArray)(const Model *self, const Attribute *attributes);

	/**
	 * @fn Buffer *Model::vertexBuffer(const Model *self, const Attribute *attributes)
	 * @param self The Model.
	 * @param attributes The tagged Attributes to select for the Buffer.
	 * @return A Buffer containing the tagged Attributes of Model's vertex data.
	 * @memberof Model
	 */
	Buffer *(*vertexBuffer)(const Model *self, const Attribute *attributes);
};

/**
 * @fn Class *Model::_Model(void)
 * @brief The Model archetype.
 * @return The Model Class.
 * @memberof Model
 */
OBJECTIVELYGL_EXPORT Class *_Model(void);
