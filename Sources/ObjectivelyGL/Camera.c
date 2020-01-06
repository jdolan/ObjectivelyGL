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

#include "Camera.h"

#define _Class _Camera

#pragma mark - Camera

/**
 * @fn void Camera::fly(Camera *self, const vec3s dir, float seconds)
 * @memberof Camera
 */
static void fly(Camera *self, const vec3s dir, float seconds) {

	float speed = glms_vec3_norm(self->velocity);

	const float acceleration = max(0, self->speed - speed) * self->acceleration * seconds;
	self->velocity = glms_vec3_add(self->velocity, glms_vec3_scale(dir, acceleration));

	speed = glms_vec3_norm(self->velocity);
	if (speed > FLT_EPSILON) {
		const float friction = max(0, speed - self->friction * seconds) / speed;
		self->velocity = glms_vec3_scale(self->velocity, friction);
	} else {
		self->velocity = glms_vec3_zero();
	}

	$(self, move, glms_vec3_scale(self->velocity, seconds));
}

/**
 * @fn void Camera::freeLook(Camera *self, int deltaX, int deltaY)
 * @memberof Camera
 */
static void freeLook(Camera *self, int deltaX, int deltaY) {

	vec3s lookAt = self->position;

	lookAt = glms_vec3_add(lookAt, glms_vec3_scale(self->forward, 200.f));
	lookAt = glms_vec3_add(lookAt, glms_vec3_scale(self->right, deltaX * self->sensitivity));
	lookAt = glms_vec3_add(lookAt, glms_vec3_scale(self->up, deltaY * self->sensitivity));

	$(self, lookAt, lookAt);
}

/**
 * @fn Camera *Camera::init(Camera *self)
 * @memberof Camera
 */
static Camera *init(Camera *self) {

	self = (Camera *) super(Object, self, init);
	if (self) {

		self->position.z = 20.f;
		self->forward.z = -1.f;
		self->up.y = 1.f;
		self->right.x = 1.f;
		self->worldUp.y = 1.f;

		self->fovY = 90.f;
		self->nearZ = 0.1f;
		self->farZ = 100.f;

		self->velocity = glms_vec3_zero();
		self->speed = 100.f;
		self->acceleration = 10.f;
		self->friction = 200.f;

		self->sensitivity = 1.f;
	}

	return self;
}

/**
 * @fn void Camera::look(Camera *self, const vec3s forward)
 * @memberof Camera
 */
static void look(Camera *self, const vec3s forward) {

	self->forward = glms_vec3_normalize(forward);
	self->right = glms_vec3_normalize(glms_vec3_cross(self->forward, self->worldUp));
	self->up = glms_vec3_normalize(glms_vec3_cross(self->forward, self->right));
}

/**
 * @fn void Camera::lookAt(Camera *self, const vec3s point)
 * @memberof Camera
 */
static void lookAt(Camera *self, const vec3s point) {
	$(self, look, glms_vec3_sub(point, self->position));
}

/**
 * @fn void Camera::move(Camera *self, const vec3s move)
 * @memberof Camera
 */
static void move(Camera *self, const vec3s move) {
	self->position = glms_vec3_add(self->position, move);
}

/**
 * @fn void Camera::moveTo(Camera *self, const vec3s position)
 * @memberof Camera
 */
static void moveTo(Camera *self, const vec3s position) {
	self->position = position;
}

/**
 * @fn mat4s Camera::perspective(const Camera *self, int width, int height)
 * @memberof Camera
 */
static mat4s perspective(const Camera *self, int width, int height) {
	return glms_perspective(glm_rad(self->fovY),
							width / (float) height,
							self->nearZ,
							self->farZ);
}

/**
 * @fn mat4s Camera::view(const Camera *self)
 * @memberof Camera
 */
static mat4s view(const Camera *self) {
	return glms_look(self->position, self->forward, self->up);
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((CameraInterface *) clazz->interface)->fly = fly;
	((CameraInterface *) clazz->interface)->freeLook = freeLook;
	((CameraInterface *) clazz->interface)->init = init;
	((CameraInterface *) clazz->interface)->look = look;
	((CameraInterface *) clazz->interface)->lookAt = lookAt;
	((CameraInterface *) clazz->interface)->move = move;
	((CameraInterface *) clazz->interface)->moveTo = moveTo;
	((CameraInterface *) clazz->interface)->perspective = perspective;
	((CameraInterface *) clazz->interface)->view = view;
}

/**
 * @fn Class *Camera::_Camera(void)
 * @memberof Camera
 */
Class *_Camera(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "Camera",
			.superclass = _Object(),
			.instanceSize = sizeof(Camera),
			.interfaceOffset = offsetof(Camera, interface),
			.interfaceSize = sizeof(CameraInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
