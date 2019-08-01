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

#include "Test.h"

static void setup(void) {
	createContext(3, 3);
}

static void teardown(void) {
	destroyContext();
}

START_TEST(initWithAttributes) {

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);

	typedef struct {
		vec3f position;
		vec2f diffuse;
		vec3f normal;
		vec3f tangent;
		vec3f bitangent;
	} Vertex;

	const Attribute attributes[] = MakeAttributes(
		VertexAttributeVec3f(0, Vertex, position),
		VertexAttributeVec2f(1, Vertex, diffuse),
		VertexAttributeVec3f(2, Vertex, normal),
		VertexAttributeVec3f(3, Vertex, tangent),
		VertexAttributeVec3f(4, Vertex, bitangent)
	);

	VertexArray *array = $(alloc(VertexArray), initWithAttributes, buffer, attributes);
	ck_assert_ptr_ne(NULL, array);
	ck_assert_int_ne(0, array->name);
	ck_assert_ptr_ne(NULL, array->attributes);
	release(array);

	release(buffer);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("VertexArray");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, initWithAttributes);

	Suite *suite = suite_create("VertexArray");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
