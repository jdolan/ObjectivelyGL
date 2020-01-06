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

#include "Scene.h"

#define _Class _Scene

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	Scene *this = (Scene *) self;

	release(this->nodes);

	super(Object, self, dealloc);
}

#pragma mark - Scene

/**
 * @fn Node *Scene::addNode(Scene *self, const Node *parent)
 * @memberof Scene
 */
static Node *addNode(Scene *self, const Node *parent) {

	Node node = {
		.position = GLMS_VEC3_ZERO,
		.angles = GLMS_VEC3_ZERO,
		.scale = GLMS_VEC3_ONE,
		.parent = parent ?: self->root,

	};

	$(self->nodes, addElement, &node);

	return VectorElement(self->nodes, Node, self->nodes->count - 1);
}

/**
 * @fn void Scene::enumerateNodes(const Scene *self, int tags, NodeEnumerator enumerator, ident data)
 * @memberof Scene
 */
static void enumerateNodes(const Scene *self, int tags, NodeEnumerator enumerator, ident data) {

	assert(enumerator);

	Node *node = self->nodes->elements;
	for (size_t i = 0; i < self->nodes->count; i++, node++) {
		if (tags) {
			if (node->tag & tags) {
				enumerator(node, data);
			}
		} else {
			enumerator(node, data);
		}
	}
}

/**
 * @fn Scene *Scene::init(Scene *self)
 * @memberof Scene
 */
static Scene *init(Scene *self) {

	self = (Scene *) super(Object, self, init);
	if (self) {
		self->nodes = $$(Vector, vectorWithSize, sizeof(Node));
		assert(self->nodes);

		self->root = $(self, addNode, NULL);
		assert(self->root);
	}

	return self;
}

/**
 * @fn void Scene::ready(Scene *self)
 * @memberof Scene
 */
static void ready(Scene *self) {

	Node *node = self->nodes->elements;
	for (size_t i = 0; i < self->nodes->count; i++, node++) {

		node->local = glms_mat4_identity();
		node->local = glms_translate(node->local, node->position);
		node->local = glms_rotate_x(node->local, glm_rad(node->angles.x));
		node->local = glms_rotate_y(node->local, glm_rad(node->angles.y));
		node->local = glms_rotate_z(node->local, glm_rad(node->angles.z));
		node->local = glms_scale(node->local, node->scale);

		node->transform = node->local;
		const Node *ancestor = node->parent;
		while (ancestor) {
			node->transform = glms_mat4_mul(ancestor->local, node->transform);
			ancestor = ancestor->parent;
		}
	}
}

/**
 * @fn void Scene::removeNode(Scene *self, const Node *node)
 * @memberof Scene
 */
static void removeNode(Scene *self, const Node *node) {

	const ssize_t index = $(self->nodes, indexOfElement, (ident) node);
	if (index > -1) {
		$(self->nodes, removeElementAtIndex, index);
	}
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((SceneInterface *) clazz->interface)->addNode = addNode;
	((SceneInterface *) clazz->interface)->enumerateNodes = enumerateNodes;
	((SceneInterface *) clazz->interface)->init = init;
	((SceneInterface *) clazz->interface)->ready = ready;
	((SceneInterface *) clazz->interface)->removeNode = removeNode;
}

/**
 * @fn Class *Scene::_Scene(void)
 * @memberof Scene
 */
Class *_Scene(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Scene",
			.superclass = _Object(),
			.instanceSize = sizeof(Scene),
			.interfaceOffset = offsetof(Scene, interface),
			.interfaceSize = sizeof(SceneInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
