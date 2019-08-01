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

#include <check.h>
#include <math.h>

#include <ObjectivelyGL.h>

#define assert_flt_eq(a, b) ck_assert(fabsf(a - b) < __FLT_EPSILON__)
#define assert_vec_eq(a, b) \
{ \
	assert_flt_eq(a.x, b.x); \
	assert_flt_eq(a.y, b.y); \
	assert_flt_eq(a.z, b.z); \
	assert_flt_eq(a.w, b.w); \
}

START_TEST(_vec0) {
	assert_vec_eq(vec4(0, 0, 0, 0), vec0());
} END_TEST

START_TEST(_vec1) {
	assert_vec_eq(vec4(1, 0, 0, 0), vec1(1));
} END_TEST

START_TEST(_vec2) {
	assert_vec_eq(vec2(1, 2), vec2(1, 2));
	assert_flt_eq(vec2(1, 2).x, 1);
	assert_flt_eq(vec2(1, 2).y, 2);
} END_TEST

START_TEST(_vec3) {
	assert_vec_eq(vec3(1, 2, 3), vec3(1, 2, 3));
	assert_flt_eq(vec3(1, 2, 3).x, 1);
	assert_flt_eq(vec3(1, 2, 3).y, 2);
	assert_flt_eq(vec3(1, 2, 3).z, 3);
} END_TEST

START_TEST(_vec4) {
	assert_vec_eq(vec4(1, 2, 3, 4), vec4(1, 2, 3, 4));
	assert_flt_eq(vec4(1, 2, 3, 4).x, 1);
	assert_flt_eq(vec4(1, 2, 3, 4).y, 2);
	assert_flt_eq(vec4(1, 2, 3, 4).z, 3);
	assert_flt_eq(vec4(1, 2, 3, 4).w, 4);
} END_TEST

START_TEST(_vec_add) {
	assert_vec_eq(vec_add(vec3(1, 2, 3), vec3(1, 1, 1)), vec3(2, 3, 4));
	assert_vec_eq(vec_add(vec3(-1, 2, 3), vec3(1, 1, 1)), vec3(0, 3, 4));
	assert_vec_eq(vec_add(vec3(1, 2, -3), vec3(1, 1, 1)), vec3(2, 3, -2));
} END_TEST

START_TEST(_vec_cmp) {
	assert_vec_eq(vec0(), vec_cmp(vec4(1, 2, 3, 4), vec4(1, 2, 3, 4)));
	ck_assert(vec_cmp(vec4(1, 2, 3, 4), vec4(0, 2, 3, 4)).x);
	ck_assert(vec_cmp(vec4(1, 2, 3, 4), vec4(1, 1, 3, 4)).y);
	ck_assert(vec_cmp(vec4(1, 2, 3, 4), vec4(1, 2, 4, 4)).z);
	ck_assert(vec_cmp(vec4(1, 2, 3, 4), vec4(1, 2, 3, 5)).w);
} END_TEST

START_TEST(_vec_cross) {
	assert_vec_eq(vec3(-3, 6, -3), vec_cross(vec3(1, 2, 3), vec3(4, 5, 6)));
} END_TEST

START_TEST(_vec_div) {
	assert_vec_eq(vec4(1, 1, 1, 1), vec_div(vec4(1, 2, 3, 1), vec4(1, 2, 3, 1)));
	assert_vec_eq(vec4(2, 2, 2, 1), vec_div(vec4(2, 4, 6, 1), vec4(1, 2, 3, 1)));
	assert_vec_eq(vec4(1, 2, 3, 1), vec_div(vec4(1, 4, 9, 1), vec4(1, 2, 3, 1)));
} END_TEST

START_TEST(_vec_dot) {
	assert_flt_eq( 1, vec_dot(vec3(1, 0, 0), vec3( 1, 0, 0)).x);
	assert_flt_eq(-1, vec_dot(vec3(1, 0, 0), vec3(-1, 0, 0)).x);
	assert_flt_eq( 0, vec_dot(vec3(1, 0, 0), vec3( 0, 1, 0)).x);
} END_TEST

START_TEST(_vec_len) {
	assert_flt_eq(1, vec_len(vec3(1, 0, 0)).x);
	assert_flt_eq(2, vec_len(vec3(2, 0, 0)).x);
	assert_flt_eq(5, vec_len(vec3(3, 0, 4)).x);
} END_TEST

START_TEST(_vec_mul) {
	assert_vec_eq(vec3(0, 0, 0), vec_mul(vec3(1, 2, 3), vec3(0, 0, 0)));
	assert_vec_eq(vec3(1, 2, 3), vec_mul(vec3(1, 2, 3), vec3(1, 1, 1)));
	assert_vec_eq(vec3(1, 4, 9), vec_mul(vec3(1, 2, 3), vec3(1, 2, 3)));
} END_TEST

START_TEST(_vec_normalize) {
	assert_vec_eq(vec3(1, 0, 0), vec_normalize(vec3(1, 0, 0)));
	assert_vec_eq(vec3(1, 0, 0), vec_normalize(vec3(2, 0, 0)));
	assert_vec_eq(vec3(1 / sqrtf(3), 1 / sqrtf(3), 1 / sqrtf(3)), vec_normalize(vec3(1, 1, 1)));
} END_TEST

START_TEST(_vec_scale) {
	assert_vec_eq(vec3(1, 1, 1), vec_scale(vec3(1, 1, 1), 1));
	assert_vec_eq(vec3(2, 4, 6), vec_scale(vec3(1, 2, 3), 2));
	assert_vec_eq(vec3(-1, -1, -1), vec_scale(vec3(1, 1, 1), -1));
} END_TEST

START_TEST(_vec_scale_add) {
	assert_vec_eq(vec3(1, 1, 1), vec_scale_add(vec3(0, 0, 0), 1, vec3(1, 1, 1)));
	assert_vec_eq(vec3(3, 6, 9), vec_scale_add(vec3(1, 2, 3), 2, vec3(1, 2, 3)));
	assert_vec_eq(vec3(0, 0, 0), vec_scale_add(vec0(), 1, vec0()));
} END_TEST

START_TEST(_vec_sqrt) {
	assert_vec_eq(vec3(1, 2, 3), vec_sqrt(vec3(1, 4, 9)));
	assert_vec_eq(vec3(4, 5, 7), vec_sqrt(vec3(16, 25, 49)));
	assert_vec_eq(vec3(sqrtf(2), sqrtf(3), sqrtf(5)), vec_sqrt(vec3(2, 3, 5)));
} END_TEST

START_TEST(_vec_sub) {
	assert_vec_eq(vec_sub(vec3(1, 2, 3), vec3(1, 1, 1)), vec3(0, 1, 2));
	assert_vec_eq(vec_sub(vec3(1, 2, 3), vec3(0, 0, 0)), vec3(1, 2, 3));
	assert_vec_eq(vec_sub(vec3(1, 2, 3), vec3(2, 3, 4)), vec3(-1, -1, -1));
} END_TEST

START_TEST(_vec_sum) {
	assert_vec_eq(vec3(0, 0, 0), vec_sum(vec3(0, 0, 0)));
	assert_vec_eq(vec3(3, 0, 0), vec_sum(vec3(1, 1, 1)));
	assert_vec_eq(vec3(6, 0, 0), vec_sum(vec3(1, 2, 3)));
} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Vector");

	tcase_add_test(tcase, _vec0);
	tcase_add_test(tcase, _vec1);
	tcase_add_test(tcase, _vec2);
	tcase_add_test(tcase, _vec3);
	tcase_add_test(tcase, _vec4);
	tcase_add_test(tcase, _vec_add);
	tcase_add_test(tcase, _vec_cmp);
	tcase_add_test(tcase, _vec_cross);
	tcase_add_test(tcase, _vec_dot);
	tcase_add_test(tcase, _vec_div);
	tcase_add_test(tcase, _vec_len);
	tcase_add_test(tcase, _vec_mul);
	tcase_add_test(tcase, _vec_normalize);
	tcase_add_test(tcase, _vec_scale);
	tcase_add_test(tcase, _vec_scale_add);
	tcase_add_test(tcase, _vec_sqrt);
	tcase_add_test(tcase, _vec_sub);
	tcase_add_test(tcase, _vec_sum);

	Suite *suite = suite_create("Vector");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
