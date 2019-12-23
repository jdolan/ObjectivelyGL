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

#include <ObjectivelyGL/CommandQueue.h>

#define _Class _CommandQueue

#pragma mark - Object

/**
 * @see Object::dealloc(Object *)
 */
static void dealloc(Object *self) {

	CommandQueue *this = (CommandQueue *) self;

	free(this->commands);

	release(this->condition);

	super(Object, self, dealloc);
}

#pragma mark - CommandQueue

/**
 * @fn _Bool CommandQueue::dequeue(void)
 * @memberof CommandQueue
 */
static _Bool dequeue(CommandQueue *self) {

	_Bool dequeued = false;

	synchronized(self->condition, {
		Command *cmd = self->commands + self->pending;
		if (cmd->consumer) {
			cmd->consumer(cmd->data);
			cmd->consumer = NULL;
			cmd->data = NULL;

			self->pending = (self->pending + 1) % self->capacity;
			dequeued = true;
		}
	});

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
		Command *cmd = &self->commands[self->free];
		if (cmd->consumer == NULL) {
			cmd->consumer = consumer;
			cmd->data = data;

			self->free = (self->free + 1) % self->capacity;
			enqueued = true;
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
	}

	return self;
}

/**
 * @fn _Bool CommandQueue::isEmpty(const CommandQueue *self)
 * @memberof CommandQueue
 */
static _Bool isEmpty(const CommandQueue *self) {

	_Bool isEmpty = false;

	synchronized(self->condition, {
		const Command *cmd = self->commands + self->pending;
		if (cmd->consumer == NULL) {
			isEmpty = true;
		}
	});

	return isEmpty;
}

/**
 * @fn void CommandQueue::waitUntilEmpty(const CommandQueue *self)
 * @memberof CommandQueue
 */
static void waitUntilEmpty(const CommandQueue *self) {

	while (!$(self, isEmpty)) {

	}
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
	((CommandQueueInterface *) clazz->interface)->initWithCapacity = initWithCapacity;
	((CommandQueueInterface *) clazz->interface)->isEmpty = isEmpty;
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
