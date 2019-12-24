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

#pragma once

#include <Objectively/Condition.h>
#include <Objectively/Thread.h>

#include <ObjectivelyGL/Types.h>

/**
 * @file
 * @brief CommandQueues allow asynchronous rendering via a dedicatd thread.
 * @details To process Commands on a dedicated background thread, use _start_ and _stop_.
 * To process Commands on the calling thread, use _dequeue_ or _flush_.
 */

typedef struct CommandQueue CommandQueue;
typedef struct CommandQueueInterface CommandQueueInterface;

typedef struct Command {
	Consumer consumer;
	ident data;
} Command;

/**
 * @brief The CommandQueue type.
 * @extends Object
 */
struct CommandQueue {

	/**
	 * @brief The superclass.
	 */
	Object object;

	/**
	 * @brief The interface.
	 * @protected
	 */
	CommandQueueInterface *interface;

	/**
	 * @private
	 */
	Command *commands;

	/**
	 * @brief The capacity.
	 */
	size_t capacity;

	/**
	 * @brief The count of pending Commands.
	 */
	size_t count;

	/**
	 * @private
	 */
	uintptr_t free, pending;

	/**
	 * @private
	 */
	Condition *condition;

	/**
	 * @private
	 */
	Thread *thread;
};

/**
 * @brief The CommandQueue interface.
 */
struct CommandQueueInterface {

	/**
	 * @brief The superclass interface.
	 */
	ObjectInterface objectInterface;

	/**
	 * @fn _Bool CommandQueue::dequeue(CommandQueue *self)
	 * @brief Dequeues and executes the next pending command on the calling thread.
	 * @param self The CommandQueue.
	 * @return True if a Command was dequeued, false otherwise.
	 * @memberof CommandQueue
	 */
	_Bool (*dequeue)(CommandQueue *self);

	/**
	 * @fn _Bool CommandQueue::enqueue(CommandQueue *self, Consumer consumer, ident data)
	 * @brief Enqueues a new Command with the given Consumer and data.
	 * @param self The CommandQueue.
	 * @param consumer The Consumer.
	 * @param data User data.
	 * @return True if the Command was successfully enqueued, false otherwise.
	 * @memberof CommandQueue
	 */
	_Bool (*enqueue)(CommandQueue *self, Consumer consumer, ident data);

	/**
	 * @fn void CommandQueue::flush(CommandQueue *self)
	 * @brief Dequeues and executes all pending Commands on the calling thread.
	 * @param self The CommandQueue.
	 * @memberof CommandQueue
	 */
	void (*flush)(CommandQueue *self);

	/**
	 * @fn CommandQueue *CommandQueue::init(CommandQueue *self)
	 * @brief Initializes this CommandQueue with a default capacity.
	 * @param self The CommandQueue.
	 * @return The initialized CommandQueue, or `NULL` on error.
	 * @memberof CommandQueue
	 */
	CommandQueue *(*init)(CommandQueue *self);

	/**
	 * @fn CommandQueue *CommandQueue::initWithCapacity(CommandQueue *self, size_t capacity)
	 * @brief Initializes this CommandQueue with the specified capacity.
	 * @param self The CommandQueue.
	 * @param capacity The capacity.
	 * @return The initialized CommandQueue, or `NULL` on error.
	 * @memberof CommandQueue
	 */
	CommandQueue *(*initWithCapacity)(CommandQueue *self, size_t capacity);

	/**
	 * @fn _Bool CommandQueue::isEmpty(const CommandQueue *self)
	 * @param self The CommandQueue.
	 * @return True if there are no pending Commands in this CommandQueue, false otherwise.
	 * @memberof CommandQueue
	 */
	_Bool (*isEmpty)(const CommandQueue *self);

	/**
	 * @fn void CommandQueue::resize(CommandQueue *self, size_t capacity)
	 * @brief Resizes this CommandQueue to the specified capacity.
	 * @param self The CommandQueue.
	 * @param capacity The desired capacity.
	 * @memberof CommandQueue
	 */
	void (*resize)(CommandQueue *self, size_t capacity);

	/**
	 * @fn void CommandQueue::start(CommandQueue *self)
	 * @brief Starts the worker Thread to flush the queue.
	 * @param self The CommandQueue.
	 * @memberof CommandQueue
	 */
	void (*start)(CommandQueue *self);

	/**
	 * @fn void *CommandQueue::start(CommandQueue *self)
	 * @brief Stops the worker Thread.
	 * @param self The CommandQueue.
	 * @memberof CommandQueue
	 */
	void (*stop)(CommandQueue *self);

	/**
	 * @fn void CommandQueue::waitUntilEmpty(const CommandQueue *self)
	 * @brief Blocks the calling thread until this CommandQueue is empty.
	 * @param self The CommandQueueue.
	 * @memberof CommandQueue
	 */
	void (*waitUntilEmpty)(const CommandQueue *self);
};

/**
 * @fn Class *CommandQueue::_CommandQueue(void)
 * @brief The CommandQueue archetype.
 * @return The CommandQueue Class.
 * @memberof CommandQueue
 */
OBJECTIVELYGL_EXPORT Class *_CommandQueue(void);
