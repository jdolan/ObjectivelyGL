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

#include <Objectively/Vector.h>

#include <ObjectivelyGL/Camera.h>
#include <ObjectivelyGL/VertexArray.h>

/**
 * @file
 * @brief ..
 */

typedef struct Scene Scene;
typedef struct SceneInterface SceneInterface;


typedef struct Node Node;

/**
 * @brief A Scene is a graph of Nodes.
 */
struct Node {

	/**
	 * @brief The parent Node, or `NULL` if this Node is the root.
	 */
	const Node *parent;

	/**
	 * @brief The position of of this Node, relative to its parent.
	 */
	vec3s position;

	/**
	 * @brief The Euler angles of this Node.
	 */
	vec3s angles;

	/**
	 * @brief The scale of this Node.
	 */
	vec3s scale;

	/**
	 * @brief The local transform of this Node, relative to its parent.
	 * @remarks This is calculated by @see Scene::ready(Scene *).
	 */
	mat4s local;

	/**
	 * @brief The transform of this node, in world space.
	 * @remarks This is calculated by @see Scene::ready(Scene *)
	 */
	mat4s transform;

	/**
	 * @brief User tag.
	 */
	int tag;

	/**
	 * @brief User data.
	 */
	ident data;
};

/**
 * @brief The Scene type.
 * @extends Object
 */
struct Scene {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	SceneInterface *interface;

	/**
	 * @brief The root Node.
	 */
	Node *root;

	/**
	 * @brief The nodes.
	 */
	Vector *nodes;
};

typedef void NodeEnumerator(const Node *node, ident data);


/**
 * @brief The Scene interface.
 */
struct SceneInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn Node *Scene::addNode(Scene *self, const Node *parent)
	 * @brief Adds a new Node to this Scene.
	 * @param self The Scene.
	 * @param parent The parent Node, or `NULL`.
	 * @return The new Node.
	 * @memberof Scene
	 */
	Node *(*addNode)(Scene *self, const Node *parent);

	/**
	 * @fn void Scene::enumerateNodes(const Scene *self, int tags, NodeEnumerator enumerator, ident data)
	 * @brief Enumerate Nodes matching the given tag mask with the specified enumerator.
	 * @param self The Scene.
	 * @param tags The tag mask, or `0` to enumerate all Nodes.
	 * @param enumerator The enumerator function.
	 * @param data User data.
	 * @memberof Scene
	 */
	void (*enumerateNodes)(const Scene *self, int tags, NodeEnumerator enumerator, ident data);

	/**
	 * @fn Scene *Scene::init(Scene *self)
	 * @brief Initializes this Scene.
	 * @param self The Scene.
	 * @return The initialized Scene, or `NULL` on error.
	 * @memberof Scene
	 */
	Scene *(*init)(Scene *self);

	/**
	 * @fn void Scene::ready(Scene *self)
	 * @brief Readies this Scene for rendering.
	 * @details This should be called once per frame, before any draw operations.
	 * @param self The Scene
	 * @memberof Scene
	 */
	void (*ready)(Scene *self);

	/**
	 * @fn void Scene::removeNode(Scene *self, const Node *node)
	 * @brief Removes the specified Node from this Scene.
	 * @param self The Scene.
	 * @param node The Node to remove.
	 * @memberof Scene
	 */
	void (*removeNode)(Scene *self, const Node *node);
};

/**
 * @fn Class *Scene::_Scene(void)
 * @brief The Scene archetype.
 * @return The Scene Class.
 * @memberof Scene
 */
OBJECTIVELY_EXPORT Class *_Scene(void);
