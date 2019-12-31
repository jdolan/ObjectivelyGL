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

START_TEST(attributes) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	ck_assert_ptr_ne(NULL, program);

	FreeProgramDescriptor(&descriptor);

	$(program, use);

	const Variable *attributes = program->attributes->elements;
	ck_assert_ptr_ne(NULL, attributes);

	const Variable *position = $(program, attributeForName, "position");
	ck_assert_int_ne(-1, position->location);
	ck_assert_str_eq("position", position->name);
	ck_assert_int_eq(1, position->size);
	ck_assert_int_eq(GL_FLOAT_VEC3, position->type);

	release(program);

} END_TEST

START_TEST(uniforms) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	ck_assert_ptr_ne(NULL, program);

	FreeProgramDescriptor(&descriptor);

	$(program, use);

	const Variable *uniforms = program->uniforms->elements;
	ck_assert_ptr_ne(NULL, uniforms);

	const Variable *projection = $(program, uniformForName, "projection");
	ck_assert_int_ne(-1, projection->location);
	ck_assert_str_eq("projection", projection->name);
	ck_assert_int_eq(1, projection->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, projection->type);

	const Variable *view = $(program, uniformForName, "view");
	ck_assert_int_ne(-1, view->location);
	ck_assert_str_eq("view", view->name);
	ck_assert_int_eq(1, view->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, view->type);

	const Variable *model = $(program, uniformForName, "model");
	ck_assert_int_ne(-1, model->location);
	ck_assert_str_eq("model", model->name);
	ck_assert_int_eq(1, model->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, model->type);

	release(program);

} END_TEST

START_TEST(infoLog) {

	Program *program = $(alloc(Program), init);
	ck_assert_ptr_ne(NULL, program);

	GLchar *info = $(program, infoLog);
	ck_assert_str_eq("", info);
	free(info);

	release(program);
} END_TEST

START_TEST(init) {

	Program *program = $(alloc(Program), init);
	ck_assert_ptr_ne(NULL, program);

	ck_assert_int_ne(0, program->name);
	ck_assert_ptr_ne(NULL, program->shaders);

	release(program);

} END_TEST

START_TEST(initWithShaders) {

	Shader *vertex = $(alloc(Shader), initWithResourceName, GL_VERTEX_SHADER, "simple.vs.glsl");
	Shader *fragment = $(alloc(Shader), initWithResourceName, GL_FRAGMENT_SHADER, "simple.fs.glsl");

	ck_assert_ptr_ne(NULL, vertex);
	ck_assert_ptr_ne(NULL, fragment);

	ck_assert_int_eq(GL_TRUE, $(vertex, compile));
	ck_assert_int_eq(GL_TRUE, $(fragment, compile));

	Program *program = $(alloc(Program), initWithShaders, vertex, fragment, NULL);
	ck_assert_ptr_ne(NULL, program);

	release(vertex);
	release(fragment);

	ck_assert($((Array *) program->shaders, containsObject, vertex));
	ck_assert($((Array *) program->shaders, containsObject, fragment));

	release(program);

} END_TEST

START_TEST(initWithDescriptor) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	ck_assert_ptr_ne(NULL, program);

	ck_assert_int_eq(2, program->shaders->array.count);

	release(program);

	FreeProgramDescriptor(&descriptor);

} END_TEST

START_TEST(initWithDescriptor_missing) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "missing.gs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	ck_assert_ptr_eq(NULL, program);

	ck_assert_ptr_ne(NULL, descriptor.shaders[0].shader);
	ck_assert_int_eq(GL_TRUE, descriptor.shaders[0].status);

	ck_assert_ptr_eq(NULL, descriptor.shaders[1].shader);
	ck_assert_int_eq(GL_FALSE, descriptor.shaders[1].status);

	release(program);

	FreeProgramDescriptor(&descriptor);

} END_TEST

START_TEST(initWithDescriptor_syntaxError) {

	ProgramDescriptor descriptor = MakeProgramDescriptor(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "syntax-error.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptor, &descriptor);
	ck_assert_ptr_eq(NULL, program);

	ck_assert_ptr_ne(NULL, descriptor.shaders[0].shader);
	ck_assert_int_eq(GL_TRUE, descriptor.shaders[0].status);

	ck_assert_ptr_eq(NULL, descriptor.shaders[1].shader);
	ck_assert_int_eq(GL_FALSE, descriptor.shaders[1].status);

	release(program);

	FreeProgramDescriptor(&descriptor);

} END_TEST

START_TEST(link) {

	Shader *vertex = $(alloc(Shader), initWithResourceName, GL_VERTEX_SHADER, "simple.vs.glsl");
	Shader *fragment = $(alloc(Shader), initWithResourceName, GL_FRAGMENT_SHADER, "simple.fs.glsl");

	ck_assert_ptr_ne(NULL, vertex);
	ck_assert_ptr_ne(NULL, fragment);

	ck_assert_int_eq(GL_TRUE, $(vertex, compile));
	ck_assert_int_eq(GL_TRUE, $(fragment, compile));

	Program *program = $(alloc(Program), initWithShaders, vertex, fragment, NULL);
	ck_assert_ptr_ne(NULL, program);

	release(vertex);
	release(fragment);

	const GLint status = $(program, link);
	ck_assert_int_eq(GL_TRUE, status);

	release(program);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Program");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, attributes);
	tcase_add_test(tcase, uniforms);
	tcase_add_test(tcase, infoLog);
	tcase_add_test(tcase, init);
	tcase_add_test(tcase, initWithShaders);
	tcase_add_test(tcase, initWithDescriptor);
	tcase_add_test(tcase, initWithDescriptor_missing);
	tcase_add_test(tcase, initWithDescriptor_syntaxError);
	tcase_add_test(tcase, link);

	Suite *suite = suite_create("Program");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
