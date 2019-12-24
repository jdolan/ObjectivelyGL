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

#include <Objectively/Thread.h>

static int criticalSection;

static void setup(void) {
	criticalSection = 0;
	createContext(3, 3);
}

static void teardown(void) {
	destroyContext();
}

static void command(ident data) {
	criticalSection++;
}

static void printq(const CommandQueue *q) {

	const Command *cmd = q->commands;
	for (size_t i = 0; i < q->capacity; i++, cmd++) {
		printf("%d %s\n", (int) cmd->data, i == q->pending ? "P" : i == q->free ? "F" : "");
	}
}

START_TEST(enqueue) {

	CommandQueue *q = $(alloc(CommandQueue), init);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < q->capacity; i++) {
		ck_assert_int_eq(true, $(q, enqueue, command, (ident) i));
	}

	ck_assert_int_eq(false, $(q, enqueue, command, NULL));
	ck_assert_int_eq(false, $(q, isEmpty));

	release(q);
} END_TEST

START_TEST(dequeue) {

	CommandQueue *q = $(alloc(CommandQueue), init);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < q->capacity; i++) {
		ck_assert_int_eq(true, $(q, enqueue, command, (ident) i));
	}

	for (size_t i = 0; i < q->capacity; i++) {
		ck_assert_int_eq(true, $(q, dequeue));
	}

	ck_assert_int_eq(false, $(q, dequeue));
	ck_assert_int_eq(true, $(q, isEmpty));

	release(q);
} END_TEST

START_TEST(flush) {

	CommandQueue *q = $(alloc(CommandQueue), init);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < q->capacity; i++) {
		ck_assert_int_eq(true, $(q, enqueue, command, (ident) i));
	}

	ck_assert_int_eq(false, $(q, isEmpty));

	$(q, flush);

	ck_assert_int_eq(true, $(q, isEmpty));

	release(q);
} END_TEST

START_TEST(resize) {

	CommandQueue *q = $(alloc(CommandQueue), initWithCapacity, 8);

	for (size_t i = 0; i < q->capacity; i++) {
		$(q, enqueue, command, (ident) i);
	}

	$(q, resize, 16);

	ck_assert_int_eq(0, q->pending);
	ck_assert_int_eq(8, q->free);
	ck_assert_int_eq(8, q->count);

	$(q, dequeue);
	$(q, dequeue);

	$(q, resize, 8);

	ck_assert_int_eq(0, q->pending);
	ck_assert_int_eq(6, q->free);
	ck_assert_int_eq(6, q->count);

	printq(q);

} END_TEST

START_TEST(start) {

	CommandQueue *q = $(alloc(CommandQueue), init);
	ck_assert_ptr_ne(NULL, q);

	$(q, start);

	for (size_t i = 0; i < q->capacity; i++) {
		ck_assert_int_eq(true, $(q, enqueue, command, NULL));
	}

	$(q, waitUntilEmpty);
	$(q, stop);

	ck_assert_int_eq(q->capacity, criticalSection);

	release(q);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("CommandQueue");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, enqueue);
	tcase_add_test(tcase, dequeue);
	tcase_add_test(tcase, flush);
	tcase_add_test(tcase, resize);
	tcase_add_test(tcase, start);

	Suite *suite = suite_create("CommandQueue");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
