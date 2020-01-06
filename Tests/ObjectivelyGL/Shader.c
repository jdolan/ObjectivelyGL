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

#define SOURCE "#version 330 core"

START_TEST(compile) {

	Shader *shader = $(alloc(Shader), initWithResourceName, GL_VERTEX_SHADER, "gouraud.vs.glsl");
	ck_assert(shader);

	const GLint status = $(shader, compile);
	ck_assert_int_eq(GL_TRUE, status);

	release(shader);

} END_TEST

START_TEST(compileError) {

	Shader *shader = $(alloc(Shader), initWithSource, GL_VERTEX_SHADER, "syntax error");
	ck_assert(shader);

	const GLint status = $(shader, compile);
	ck_assert_int_eq(GL_FALSE, status);

	release(shader);

} END_TEST

START_TEST(infoLog) {

	Shader *shader = $(alloc(Shader), initWithType, GL_VERTEX_SHADER);
	ck_assert_ptr_ne(NULL, shader);

	GLchar *info = $(shader, infoLog);
	ck_assert_str_eq("", info);
	free(info);

	release(shader);

} END_TEST

START_TEST(initWithBytes) {

	Shader *shader = $(alloc(Shader), initWithBytes, GL_VERTEX_SHADER, (const uint8_t *) SOURCE, strlen(SOURCE));
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_eq(SOURCE, shader->source);
	release(shader);

} END_TEST

START_TEST(initWithData) {

	Data *data = $(alloc(Data), initWithBytes, (const uint8_t *) SOURCE, strlen(SOURCE));
	ck_assert(data);

	Shader *shader = $(alloc(Shader), initWithData, GL_VERTEX_SHADER, data);
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_eq(SOURCE, shader->source);
	release(shader);

	release(data);

} END_TEST

START_TEST(initWithDescriptor) {

	ShaderDescriptor descriptor = MakeShaderDescriptor(GL_VERTEX_SHADER, "gouraud.vs.glsl");

	Shader *shader = $(alloc(Shader), initWithDescriptor, &descriptor);
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_ne("", shader->source);
	ck_assert_ptr_eq(shader, descriptor.shader);
	release(shader);

	FreeShaderDescriptor(&descriptor);

} END_TEST

START_TEST(initWithResource) {

	Resource *resource = $(alloc(Resource), initWithBytes, (uint8_t *) SOURCE, strlen(SOURCE), __FILE__);
	ck_assert(resource);

	Shader *shader = $(alloc(Shader), initWithResource, GL_VERTEX_SHADER, resource);
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_eq(SOURCE, shader->source);
	release(shader);

	release(resource);

} END_TEST

START_TEST(initWithResourceName) {

	Shader *shader = $(alloc(Shader), initWithResourceName, GL_VERTEX_SHADER, "gouraud.vs.glsl");
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_ne("", shader->source);
	release(shader);

} END_TEST

START_TEST(initWithSource) {

	Shader *shader = $(alloc(Shader), initWithSource, GL_VERTEX_SHADER, SOURCE);
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_eq(SOURCE, shader->source);
	release(shader);

} END_TEST

START_TEST(initWithType) {

	Shader *shader = $(alloc(Shader), initWithType, GL_VERTEX_SHADER);
	ck_assert(shader);
	ck_assert_int_ne(0, shader->name);
	ck_assert_int_eq(GL_VERTEX_SHADER, shader->type);
	ck_assert_str_eq("", shader->source);
	release(shader);
	
} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Shader");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, compile);
	tcase_add_test(tcase, compileError);
	tcase_add_test(tcase, infoLog);
	tcase_add_test(tcase, initWithBytes);
	tcase_add_test(tcase, initWithData);
	tcase_add_test(tcase, initWithDescriptor);
	tcase_add_test(tcase, initWithResource);
	tcase_add_test(tcase, initWithResourceName);
	tcase_add_test(tcase, initWithSource);
	tcase_add_test(tcase, initWithType);

	Suite *suite = suite_create("Shader");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
