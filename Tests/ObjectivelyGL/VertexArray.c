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

START_TEST(bind) {

	VertexArray *array = $(alloc(VertexArray), initWithAttributes, NULL);
	ck_assert_ptr_ne(NULL, array);

	$(array, bind);

	GLint name;
	glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &name);
	ck_assert_int_eq(name, array->name);

	release(array);

} END_TEST

START_TEST(initWithAttributes) {

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);

	$(buffer, bind, GL_ARRAY_BUFFER);

	typedef struct {
		GLfloat position[3];
		GLfloat diffuse[2];
		GLfloat normal[3];
		GLfloat tangent[3];
		GLfloat bitangent[3];
	} Vertex;

	const Attribute attributes[] = MakeAttributes(
		MakeAttribute(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, position)),
		MakeAttribute(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, diffuse)),
		MakeAttribute(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, normal)),
		MakeAttribute(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, tangent)),
		MakeAttribute(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), offsetof(Vertex, bitangent))
	);

	VertexArray *array = $(alloc(VertexArray), initWithAttributes, attributes);
	ck_assert_ptr_ne(NULL, array);
	ck_assert_int_ne(0, array->name);
	ck_assert_ptr_ne(NULL, array->attributes);
	release(array);

	release(buffer);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("VertexArray");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, bind);
	tcase_add_test(tcase, initWithAttributes);

	Suite *suite = suite_create("VertexArray");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
