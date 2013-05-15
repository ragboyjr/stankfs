//
//  list.h
//  link_list
//
//  Created by RJ Garcia on 4/16/13.
//  Copyright (c) 2013 RJ Garcia. All rights reserved.
//

#ifndef link_list_list_h
#define link_list_list_h

#include <assert.h>

struct list_item {
	void * data;
	struct list_item * next;
	struct list_item * prev;
};

struct list {
	
	struct list_item * head;
	struct list_item * tail;
	
	/* cur is used for iteration */
	struct list_item * cur;
	int cur_index;
	int size;
};

/*
 * NOTE
 * This is a pretty simple implementation of a doubly linked list in c.
 * We can add functions as necessary, but I think this should be fine for now.
 *
 * TODO
 * - We need to add better error handling for out of memory errors and whatnot
 * - We need to add a listtest.c in the test directory
 *
 * RJ - 2013-04-16 21:45:05
 */

/*
 * Creates a new doubly linked list
 *
 *		struct list * ll = list_create();
 *
 */
struct list * list_create();

/*
 * Initializes a doubly linked list.
 * Same as list_create, except, list_init
 * expects a created link list
 *
 *		struct list ll;
 *		list_init(&ll);
 */
void list_init(struct list *);

/*
 * Adds a list item to the end of the list
 *
 *		struct list * ll;
 *		void * data;
 *		list_add(ll, data);
 *
 */
int list_add(struct list *, void *);

/*
 * Joins the second list with the first, the second list is appended
 * to the first. Returns a pointer to the first list. Calling list_join
 * will render the second list useless, so only call this if you no longer need
 * the second list
 *
 *		struct list * ll_0, ll_1
 *		struct list * joined = list_join(ll_0, ll_1);
 */
struct list * list_join(struct list *, struct list *);

/*
 * To iterate over the linked list, run code like so
 *
 *		struct list * ll;
 *		void * data;
 *		while (list_next(ll, &data)) {
 *			// use data pointer
 *		}
 *
 */
int list_next(struct list *, void **);

/*
 * Removes the list item the iterator is pointing to.
 *
 *		struct list * ll;
 *		void * data;
 *		while (list_next(ll, &data)) {
 *			// the following is sudo code
 *			if (data == SOME_VALUE) {
 *				// the current list item will be freed up from the list, and the
 *				// iterator now points to the element before current.
 *				list_remove_current(ll);
 *			}
 *		}
 */
int list_remove_current(struct list *);

/*
 * Resets the iteration pointer always
 *
 *		struct list * ll;
 *		list_reset(ll);
 *		// now we can iterate trough again
 *
 */
void list_reset_hard(struct list *);

/*
 * Resets the iteration pointer only if it's reached the end of the list
 *
 *		struct list * ll;
 *		list_reset(ll);
 *		// now we can iterate trough again
 *
 */
void list_reset(struct list *);

/*
 * Frees up the memory for the list
 */
void list_destroy(struct list *);

#define list_foreach(ll, data)	list_reset_hard(ll);\
	assert(ll);assert(data);\
	while (list_next(ll, data))

#endif
