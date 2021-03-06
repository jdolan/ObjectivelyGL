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

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "CommandQueue.h"

#define _Class _CommandQueue

#define COMMAND_QUEUE_DEFAULT_CAPACITY 64

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	CommandQueue *this = (CommandQueue *) self;

	free(this->commands);

	release(this->condition);
	release(this->thread);

	super(Object, self, dealloc);
}

#pragma mark - CommandQueue

/**
 * @fn _Bool CommandQueue::dequeue(CommandQueue *self)
 * @memberof CommandQueue
 */
static _Bool dequeue(CommandQueue *self) {
	_Bool dequeued = false;

	Command copy = { .consumer = NULL, .data = NULL };

	synchronized(self->condition, {
		Command *cmd = self->commands + self->pending;
		if (cmd->consumer) {
			copy = *cmd;
			cmd->consumer = NULL;
			cmd->data = NULL;

			self->pending = (self->pending + 1) % self->capacity;
			self->count--;
			dequeued = true;
		}
	});

	if (copy.consumer) {
		copy.consumer(copy.data);
		synchronized(self->condition, $(self->condition, signal));
	}

	return dequeued;
}

/**
 * @fn _Bool CommandQueue::enqueue(CommandQueue *self, Consumer consumer, ident data)
 * @memberof CommandQueue
 */
static _Bool enqueue(CommandQueue *self, Consumer consumer, ident data) {

	assert(consumer);

	_Bool enqueued = false;

	synchronized(self->condition, {
		Command *cmd = self->commands + self->free;
		if (cmd->consumer == NULL) {
			cmd->consumer = consumer;
			cmd->data = data;

			self->free = (self->free + 1) % self->capacity;
			self->count++;
			enqueued = true;

			$(self->condition, signal);
		}
	});

	return enqueued;
}

/**
 * @fn void CommandQueue::flush(CommandQueue *self)
 * @memberof CommandQueue
 */
static void flush(CommandQueue *self) {

	while ($(self, dequeue)) {

	}
}

/**
 * @fn CommandQueue *CommandQueue::init(CommandQueue *self)
 * @memberof CommandQueue
 */
static CommandQueue *init(CommandQueue *self) {
	return $(self, initWithCapacity, COMMAND_QUEUE_DEFAULT_CAPACITY);
}

/**
 * @brief ThreadFunction.
 */
static ident _thread(Thread *thread) {

	CommandQueue *self = thread->data;

	while (!thread->isCancelled) {

		$(self, flush);

		synchronized(self->condition, $(self->condition, wait));
	}

	return NULL;
}

/**
 * @fn CommandQueue *CommandQueue::init(CommandQueue *self, ident context)
 * @memberof CommandQueue
 */
static CommandQueue *initWithCapacity(CommandQueue *self, size_t capacity) {

	self = (CommandQueue *) super(Object, self, init);
	if (self) {
		self->capacity = capacity;
		assert(self->capacity);

		self->commands = calloc(self->capacity, sizeof(Command));
		assert(self->commands);

		self->condition = $(alloc(Condition), init);
		assert(self->condition);

		self->thread = $(alloc(Thread), initWithFunction, _thread, self);
		assert(self->thread);
	}

	return self;
}

/**
 * @fn _Bool CommandQueue::isEmpty(const CommandQueue *self)
 * @memberof CommandQueue
 */
static _Bool isEmpty(const CommandQueue *self) {
	return self->count == 0;
}

/**
 * @fn void CommandQueue::resize(CommandQueue *self, size_t capacity)
 * @memberof CommandQueue
 */
static void resize(CommandQueue *self, size_t capacity) {

	synchronized(self->condition, {

		Command *commands = calloc(capacity, sizeof(Command));
		assert(commands);

		for (size_t i = 0; i < self->count; i++) {
			*(commands + i) = *(self->commands + (self->pending + i) % self->capacity);
		}

		free(self->commands);

		self->commands = commands;
		self->capacity = capacity;
		self->pending = 0;
		self->free = self->count;
	});
}

/**
 * @fn void CommandQueue::start(CommandQueue *self)
 * @memberof CommandQueue
 */
static void start(CommandQueue *self) {

	$(self->thread, start);
}

/**
 * @fn void *CommandQueue::start(CommandQueue *self)
 * @memberof CommandQueue
 */
static void stop(CommandQueue *self) {

	$(self->thread, cancel);

	synchronized(self->condition, $(self->condition, signal));

	$(self->thread, join, NULL);
}

/**
 * @fn void CommandQueue::waitUntilEmpty(const CommandQueue *self)
 * @memberof CommandQueue
 */
static void waitUntilEmpty(const CommandQueue *self) {

	synchronized(self->condition, {
		while (self->count) {
			$(self->condition, wait);
		}
	});
}

#pragma mark - Class lifecycle

/**
 * @see Class::initialize(Class *)
 */
static void initialize(Class *clazz) {

	((ObjectInterface *) clazz->interface)->dealloc = dealloc;

	((CommandQueueInterface *) clazz->interface)->dequeue = dequeue;
	((CommandQueueInterface *) clazz->interface)->enqueue = enqueue;
	((CommandQueueInterface *) clazz->interface)->flush = flush;
	((CommandQueueInterface *) clazz->interface)->init = init;
	((CommandQueueInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((CommandQueueInterface *) clazz->interface)->isEmpty = isEmpty;
	((CommandQueueInterface *) clazz->interface)->resize = resize;
	((CommandQueueInterface *) clazz->interface)->start = start;
	((CommandQueueInterface *) clazz->interface)->stop = stop;
	((CommandQueueInterface *) clazz->interface)->waitUntilEmpty = waitUntilEmpty;
}

/**
 * @fn Class *CommandQueue::_CommandQueue(void)
 * @memberof CommandQueue
 */
Class *_CommandQueue(void) {
	static Class *clazz;
	static Once once;

	do_once(&once, {
		clazz = _initialize(&(const ClassDef) {
			.name = "CommandQueue",
			.superclass = _Object(),
			.instanceSize = sizeof(CommandQueue),
			.interfaceOffset = offsetof(CommandQueue, interface),
			.interfaceSize = sizeof(CommandQueueInterface),
			.initialize = initialize,
		});
	});

	return clazz;
}

#undef _Class
