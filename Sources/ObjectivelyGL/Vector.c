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

#include "Vector.h"

vec vec0(void) {
	return (vec) _mm_setzero_ps();
}

vec vec1(float x) {
	return (vec) _mm_set_ss(x);
}

vec vec2(float x, float y) {
	return vec3(x, y, 0);
}

vec vec3(float x, float y, float z) {
	return vec4(x, y, z, 0);
}

vec vec4(float x, float y, float z, float w) {
	return (vec) _mm_set_ps(w, z, y, x);
}

vec vec_add(const vec a, const vec b) {
	return (vec) _mm_add_ps(a.vec, b.vec);
}

vec vec_cmp(const vec a, const vec b) {
	return (vec) _mm_cmpneq_ps(a.vec, b.vec);
}

vec vec_cross(const vec a, const vec b) {
	// https://www.mathsisfun.com/algebra/vectors-cross-product.html
	return vec_sub(
		vec_mul(vec3(a.y, a.z, a.x), vec3(b.z, b.x, b.y)),
		vec_mul(vec3(a.z, a.x, a.y), vec3(b.y, b.z, b.x))
	);
}

vec vec_div(const vec a, const vec b) {
	return (vec) _mm_div_ps(a.vec, b.vec);
}

vec vec_dot(const vec a, const vec b) {
	return (vec) _mm_dp_ps(a.vec, b.vec, 0x7F);
}

vec vec_len(const vec v) {
	return (vec) _mm_sqrt_ss(vec_dot(v, v).vec);
}

vec vec_mul(const vec a, const vec b) {
	return (vec) _mm_mul_ps(a.vec, b.vec);
}

vec vec_new(float f) {
	return (vec) _mm_set1_ps(f);
}

vec vec_normalize(const vec v) {
	return vec_div(v, vec_sqrt(vec_dot(v, v)));
}

vec vec_scale(const vec v, float scale) {
	return vec_mul(v, vec_new(scale));
}

vec vec_scale_add(const vec a, float scale, const vec b) {
	return vec_add(a, vec_scale(b, scale));
}

vec vec_sqrt(const vec v) {
	return (vec) _mm_sqrt_ps(v.vec);
}

vec vec_sub(const vec a, const vec b) {
	return (vec) _mm_sub_ps(a.vec, b.vec);
}

vec vec_sum(const vec v) {
	return vec1(v.x + v.y + v.z + v.w);
}
