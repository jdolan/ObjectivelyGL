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

#define CAPACITY 10000

static int criticalSection;

static void setup(void) {
	criticalSection = 0;
	createContext(3, 3);
}

static void teardown(void) {
	destroyContext();
}

static void cmd(ident data) {
	criticalSection++;
}

START_TEST(enqueue) {

	CommandQueue *q = $(alloc(CommandQueue), initWithCapacity, CAPACITY);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < CAPACITY; i++) {
		ck_assert_int_eq(true, $(q, enqueue, cmd, (ident) i));
	}

	ck_assert_int_eq(false, $(q, enqueue, cmd, (ident) CAPACITY));
	ck_assert_int_eq(false, $(q, isEmpty));

	release(q);
} END_TEST

START_TEST(dequeue) {

	CommandQueue *q = $(alloc(CommandQueue), initWithCapacity, CAPACITY);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < CAPACITY; i++) {
		ck_assert_int_eq(true, $(q, enqueue, cmd, (ident) i));
	}

	for (size_t i = 0; i < CAPACITY; i++) {
		ck_assert_int_eq(true, $(q, dequeue));
	}

	ck_assert_int_eq(false, $(q, dequeue));
	ck_assert_int_eq(true, $(q, isEmpty));

	release(q);
} END_TEST

START_TEST(flush) {

	CommandQueue *q = $(alloc(CommandQueue), initWithCapacity, CAPACITY);
	ck_assert_ptr_ne(NULL, q);

	for (size_t i = 0; i < CAPACITY; i++) {
		ck_assert_int_eq(true, $(q, enqueue, cmd, (ident) i));
	}

	ck_assert_int_eq(false, $(q, isEmpty));

	$(q, flush);

	ck_assert_int_eq(true, $(q, isEmpty));

	release(q);
} END_TEST

static ident run(Thread *thread) {

	CommandQueue *q = thread->data;
	while (!thread->isCancelled) {
		$(q, flush);
	}

	return NULL;
}

START_TEST(threaded) {

	CommandQueue *q = $(alloc(CommandQueue), initWithCapacity, CAPACITY);
	ck_assert_ptr_ne(NULL, q);

	Thread *thread = $(alloc(Thread), initWithFunction, run, q);
	$(thread, start);

	for (size_t i = 0; i < CAPACITY; i++) {
		ck_assert_int_eq(true, $(q, enqueue, cmd, (ident) i));
	}

	$(q, waitUntilEmpty);

	$(thread, cancel);
	$(thread, join, NULL);

	ck_assert_int_eq(CAPACITY, criticalSection);

	release(q);
	release(thread);

} END_TEST

int main(int argc, char **argv) {

	TCase *tcase = tcase_create("CommandQueue");
	tcase_add_checked_fixture(tcase, setup, teardown);

	tcase_add_test(tcase, enqueue);
	tcase_add_test(tcase, dequeue);
	tcase_add_test(tcase, flush);
	tcase_add_test(tcase, threaded);

	Suite *suite = suite_create("CommandQueue");
	suite_add_tcase(suite, tcase);

	SRunner *runner = srunner_create(suite);

	srunner_run_all(runner, CK_VERBOSE);
	int failed = srunner_ntests_failed(runner);

	srunner_free(runner);

	return failed;
}
