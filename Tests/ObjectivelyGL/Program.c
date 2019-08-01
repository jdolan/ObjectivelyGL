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

START_TEST(activeAttributes) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_ne(NULL, program);

	FreeShaderDescriptors(descriptors);

	$(program, link);
	$(program, use);

	Variable *attributes = $(program, activeAttributes);
	ck_assert_ptr_ne(NULL, attributes);

	const Variable *position = &attributes[0];
	ck_assert_int_eq(0, position->index);
	ck_assert_str_eq("position", position->name);
	ck_assert_int_eq(1, position->size);
	ck_assert_int_eq(GL_FLOAT_VEC3, position->type);

	free(attributes);
	
	release(program);

} END_TEST

START_TEST(activeUniforms) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_ne(NULL, program);

	FreeShaderDescriptors(descriptors);

	$(program, link);
	$(program, use);

	Variable *uniforms = $(program, activeUniforms);
	ck_assert_ptr_ne(NULL, uniforms);

	const Variable *projection = &uniforms[0];
	ck_assert_int_eq(0, projection->index);
	ck_assert_str_eq("Matrix.projection", projection->name);
	ck_assert_int_eq(1, projection->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, projection->type);

	const Variable *view = &uniforms[1];
	ck_assert_int_eq(1, view->index);
	ck_assert_str_eq("Matrix.view", view->name);
	ck_assert_int_eq(1, view->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, view->type);

	const Variable *model = &uniforms[2];
	ck_assert_int_eq(2, model->index);
	ck_assert_str_eq("Matrix.model", model->name);
	ck_assert_int_eq(1, model->size);
	ck_assert_int_eq(GL_FLOAT_MAT4, model->type);

	free(uniforms);

	release(program);

} END_TEST

START_TEST(infoLog) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_ne(NULL, program);

	FreeShaderDescriptors(descriptors);

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

START_TEST(initWithDescriptors) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_ne(NULL, program);

	ck_assert_int_eq(2, program->shaders->array.count);

	release(program);

} END_TEST

START_TEST(initWithDescriptors_missing) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "missing.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_eq(NULL, program);

	ck_assert_ptr_ne(NULL, descriptors[0].shader);
	ck_assert_int_eq(GL_TRUE, descriptors[0].status);

	ck_assert_ptr_eq(NULL, descriptors[1].shader);
	ck_assert_int_eq(GL_FALSE, descriptors[1].status);

	FreeShaderDescriptors(descriptors);

} END_TEST

START_TEST(initWithDescriptors_syntaxError) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "syntax-error.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_eq(NULL, program);

	ck_assert_ptr_ne(NULL, descriptors[0].shader);
	ck_assert_int_eq(GL_TRUE, descriptors[0].status);

	ck_assert_ptr_ne(NULL, descriptors[1].shader);
	ck_assert_int_eq(GL_FALSE, descriptors[1].status);

	FreeShaderDescriptors(descriptors);

} END_TEST

START_TEST(link) {

	ShaderDescriptor descriptors[] = MakeShaderDescriptors(
		MakeShaderDescriptor(GL_VERTEX_SHADER, "simple.vs.glsl"),
		MakeShaderDescriptor(GL_FRAGMENT_SHADER, "simple.fs.glsl")
	);

	Program *program = $(alloc(Program), initWithDescriptors, descriptors);
	ck_assert_ptr_ne(NULL, program);

	FreeShaderDescriptors(descriptors);

	const GLint status = $(program, link);
	ck_assert_int_eq(GL_TRUE, status);

	release(program);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Program");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, activeAttributes);
	tcase_add_test(tcase, activeUniforms);
	tcase_add_test(tcase, infoLog);
	tcase_add_test(tcase, init);
	tcase_add_test(tcase, initWithShaders);
	tcase_add_test(tcase, initWithDescriptors);
	tcase_add_test(tcase, initWithDescriptors_missing);
	tcase_add_test(tcase, initWithDescriptors_syntaxError);
	tcase_add_test(tcase, link);

	Suite *suite = suite_create("Program");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
