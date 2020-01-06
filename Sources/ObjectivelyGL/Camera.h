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

typedef struct Camera Camera;
typedef struct CameraInterface CameraInterface;

/**
 * @brief The Camera type.
 * @extends Object
 */
struct Camera {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	CameraInterface *interface;

	/**
	 * @brief The eye position.
	 */
	vec3s position;

	/**
	 * @brief The forward directional vector.
	 */
	vec3s forward;

	/**
	 * @brief The right directional vector.
	 */
	vec3s right;

	/**
	 * @brief The up directional vector.
	 */
	vec3s up;

	/**
	 * @brief The up directional vector for the world.
	 */
	vec3s worldUp;

	/**
	 * @brief The velocity for flying movement.
	 */
	vec3s velocity;

	/**
	 * @brief The maximum speed for flying movement.
	 */
	float speed;

	/**
	 * @brief The acceleration for flying movement.
	 */
	float acceleration;

	/**
	 * @brief The friction for flying movement.
	 */
	float friction;

	/**
	 * @brief The horizontal field of view, in degrees.
	 */
	float fovY;

	/**
	 * @brief The near Z clipping distance.
	 */
	float nearZ;

	/**
	 * @brief The far Z clipping distance.
	 */
	float farZ;
};

/**
 * @brief The Camera interface.
 */
struct CameraInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn void Camera::fly(Camera *self, const vec3s dir, float seconds)
	 * @brief Accelerate this camera in the specified direction for the given duration.
	 * @param self The Camera.
	 * @param dir The acceleration direction.
	 * @param seconds The duration of the movement, in seconds.
	 * @memberof Camera
	 */
	void (*fly)(Camera *self, const vec3s dir, float seconds);

	/**
	 * @fn Camera *Camera::init(Camera *self)
	 * @brief Initializes this Camera.
	 * @param self The Camera.
	 * @return The initialized Camera, or `NULL` on error.
	 * @memberof Camera
	 */
	Camera *(*init)(Camera *self);

	/**
	 * @fn void Camera::look(Camera *self, const vec3s forward)
	 * @brief Sets the forward direction of this Camera to the given vector.
	 * @param self The Camera.
	 * @param forward The forward directional vector.
	 * @memberof Camera
	 */
	void (*look)(Camera *self, const vec3s forward);

	/**
	 * @fn void Camera::lookAt(Camera *self, const vec3s point)
	 * @brief Sets the forward direction of this Camera torwards the given point.
	 * @param self The Camera.
	 * @param point The point to look at.
	 * @memberof Camera
	 */
	void (*lookAt)(Camera *self, const vec3s point);

	/**
	 * @fn void Camera::move(Camera *self, const vec3s move)
	 * @brief Adds the given vector to this Camera's position.
	 * @param self The Camera.
	 * @param move The move.
	 * @memberof Camera
	 */
	void (*move)(Camera *self, const vec3s move);

	/**
	 * @fn void Camera::moveTo(Camera *self, const vec3s position)
	 * @brief Sets this Camera's position to the specified vector.
	 * @param self The Camera.
	 * @param position The position.
	 * @memberof Camera
	 */
	void (*moveTo)(Camera *self, const vec3s position);

	/**
	 * @fn mat4s Camera::perspective(const Camera *self, int width, int height)
	 * @brief Calculates the perspective projection matrix for this Camera.
	 * @param self The Camera.
	 * @param width The viewport width.
	 * @param height The viewport height.
	 * @return The perspective projection matrix.
	 * @memberof Camera
	 */
	mat4s (*perspective)(const Camera *self, int width, int height);

	/**
	 * @fn mat4s Camera::view(const Camera *self)
	 * @brief Calculates the view matrix for this Camera.
	 * @param self The Camera.
	 * @return The view matrix.
	 * @memberof Camera
	 */
	mat4s (*view)(const Camera *self);
};

/**
 * @fn Class *Camera::_Camera(void)
 * @brief The Camera archetype.
 * @return The Camera Class.
 * @memberof Camera
 */
OBJECTIVELYGL_EXPORT Class *_Camera(void);
