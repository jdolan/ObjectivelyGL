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

#include <ObjectivelyGL/Types.h>

#include <immintrin.h>

/**
 * @brief Two component vector type.
 */
typedef union {
	/**
	 * @brief The vector.
	 */
	float vec[2];

	/**
	 * @brief Component accessors.
	 */
	struct {
		float x, y;
	};

	/**
	 * @brief Tangent space component accessors.
	 */
	struct {
		float s, t;
	};
} vec2f;

/**
 * @brief Three component vector type.
 */
typedef union {
	/**
	 * @brief The vector.
	 */
	float vec[3];

	/**
	 * @brief Component accessors.
	 */
	struct {
		float x, y, z;
	};

	/**
	 * @brief Tangent space component accessors.
	 */
	struct {
		float s, t, u;
	};

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f xy;

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f st;
} vec3f;

/**
 * @brief Four component vector type.
 */
typedef union {
	/**
	 * @brief The vector.
	 */
	float vec[4];

	/**
	 * @brief Component accessors.
	 */
	struct {
		float x, y, z, w;
	};

	/**
	 * @brief Tangent space component accessors.
	 */
	struct {
		float s, t, u, v;
	};

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f xy;

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f st;

	/**
	 * @brief Swizzle accessor.
	 */
	vec3f xyz;

	/**
	 * @brief Swizzle accessor.
	 */
	vec3f stu;
} vec4f;

/**
 * @brief Four component SSE vector type, which must be 16-bit aligned.
 */
typedef union {
	/**
	 * @brief The 128 bit SSE vector.
	 */
	__m128 vec;

	/**
	 * @brief Component accessors.
	 */
	struct {
		float x, y, z, w;
	};

	/**
	 * @brief Tangent space component accessors.
	 */
	struct {
		float s, t, u, v;
	};

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f xy;

	/**
	 * @brief Swizzle accessor.
	 */
	vec2f st;

	/**
	 * @brief Swizzle accessor.
	 */
	vec3f xyz;

	/**
	 * @brief Swizzle accessor.
	 */
	vec3f stu;

	/**
	 * @brief Swizzle accessor.
	 */
	vec4f xyzw;

	/**
	 * @brief Swizzle accessor.
	 */
	vec4f stuv;
} vec;

OBJECTIVELYGL_EXPORT vec vec0(void);
OBJECTIVELYGL_EXPORT vec vec1(float x);
OBJECTIVELYGL_EXPORT vec vec2(float x, float y);
OBJECTIVELYGL_EXPORT vec vec3(float x, float y, float z);
OBJECTIVELYGL_EXPORT vec vec4(float x, float y, float z, float w);
OBJECTIVELYGL_EXPORT vec vec_add(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_cmp(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_cross(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_div(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_dot(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_len(const vec v);
OBJECTIVELYGL_EXPORT vec vec_mul(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_new(float f);
OBJECTIVELYGL_EXPORT vec vec_normalize(const vec v);
OBJECTIVELYGL_EXPORT vec vec_scale(const vec v, float scale);
OBJECTIVELYGL_EXPORT vec vec_scale_add(const vec a, float scale, const vec b);
OBJECTIVELYGL_EXPORT vec vec_sqrt(const vec v);
OBJECTIVELYGL_EXPORT vec vec_sub(const vec a, const vec b);
OBJECTIVELYGL_EXPORT vec vec_sum(const vec v);
