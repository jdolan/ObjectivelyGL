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

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);

	$(buffer, bind, GL_ARRAY_BUFFER);

	GLint name;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &name);
	ck_assert_int_eq(name, buffer->name);

	release(buffer);

} END_TEST

START_TEST(init) {

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);
	ck_assert_int_ne(0, buffer->name);
	release(buffer);

} END_TEST

START_TEST(initWithData0) {

	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER, 0, NULL, GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), initWithData, &data);
	ck_assert_ptr_ne(NULL, buffer);
	ck_assert_int_ne(0, buffer->name);
	ck_assert_int_eq(data.size, buffer->size);
	ck_assert_int_eq(data.usage, buffer->usage);
	release(buffer);

} END_TEST

START_TEST(initWithData) {

	const GLfloat vertex[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), initWithData, &data);
	ck_assert_ptr_ne(NULL, buffer);
	ck_assert_int_ne(0, buffer->name);
	ck_assert_int_eq(data.size, buffer->size);
	ck_assert_int_eq(data.usage, buffer->usage);
	release(buffer);

} END_TEST

START_TEST(unbind) {

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);

	$(buffer, bind, GL_ARRAY_BUFFER);
	$(buffer, unbind, GL_ARRAY_BUFFER);

	GLint name;
	glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &name);
	ck_assert_int_eq(0, name);

	release(buffer);

} END_TEST

START_TEST(writeData) {

	const GLfloat vertex[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), init);
	ck_assert_ptr_ne(NULL, buffer);

	$(buffer, bind, data.target);
	$(buffer, writeData, &data);

	ck_assert_int_eq(data.size, buffer->size);
	ck_assert_int_eq(data.usage, buffer->usage);

	release(buffer);

} END_TEST

START_TEST(writeSubData) {

	const GLfloat vertex[] = { 0.0, 0.0, 0.0, 0.0, 1.0, 0.0 };
	const BufferData data = MakeBufferData(GL_ARRAY_BUFFER, sizeof(vertex), vertex, GL_STATIC_DRAW);

	Buffer *buffer = $(alloc(Buffer), initWithData, &data);
	ck_assert_ptr_ne(NULL, buffer);

	$(buffer, writeSubData, &MakeBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertex), vertex));

	release(buffer);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("Buffer");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, bind);
	tcase_add_test(tcase, init);
	tcase_add_test(tcase, initWithData0);
	tcase_add_test(tcase, initWithData);
	tcase_add_test(tcase, unbind);
	tcase_add_test(tcase, writeData);
	tcase_add_test(tcase, writeSubData);

	Suite *suite = suite_create("Buffer");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
