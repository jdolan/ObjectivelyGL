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

START_TEST(initWithShaders) {

	Shader *vertex = $(alloc(Shader), initWithDescriptor, &MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"));
	ck_assert_ptr_ne(NULL, vertex);

	Shader *fragment = $(alloc(Shader), initWithDescriptor, &MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl"));
	ck_assert_ptr_ne(NULL, fragment);

	Program *program = $(alloc(Program), initWithShaders, vertex, fragment, NULL);
	ck_assert_ptr_ne(NULL, program);

	release(vertex);
	release(fragment);

	ck_assert($((Array *) program->shaders, containsObject, vertex));
	ck_assert($((Array *) program->shaders, containsObject, fragment));

	release(program);

} END_TEST

START_TEST(initWithDescriptors) {

	ShaderDescriptor shaders[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, shaders);
	ck_assert_ptr_ne(NULL, program);

	ck_assert_int_eq(2, program->shaders->array.count);

	release(program);

} END_TEST

START_TEST(link) {

	ShaderDescriptor shaders[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, shaders);
	ck_assert_ptr_ne(NULL, program);

	const GLint status = $(program, link);
	ck_assert_int_eq(GL_TRUE, status);

	release(program);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Program");
	tcase_add_checked_fixture(tcase, setup, teardown);
	
	tcase_add_test(tcase, initWithShaders);
	tcase_add_test(tcase, initWithDescriptors);
	tcase_add_test(tcase, link);

	Suite *suite = suite_create("Program");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
