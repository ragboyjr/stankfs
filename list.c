//
//  list.c
//  link_list
//
//  Created by RJ Garcia on 4/16/13.
//  Copyright (c) 2013 RJ Garcia. All rights reserved.
//

#include "list.h"
#include <errno.h>
#include <stdlib.h>

struct list * list_create()
{
	// ll = linked_list
	struct list * ll = (struct list *) malloc(sizeof(struct list));
	
	if (ll == NULL)
	{
		return NULL;
	}
	
	ll->size		= 0;
	ll->head		= NULL;
	ll->tail		= NULL;
	ll->cur			= NULL;
	ll->cur_index	= 0;
	
	return ll;
}

void list_init(struct list * ll)
{
	assert(ll);
	
	ll->size		= 0;
	ll->head		= NULL;
	ll->tail		= NULL;
	ll->cur			= NULL;
	ll->cur_index	= 0;
}

int list_add(struct list * ll, void * data)
{
	struct list_item * li = (struct list_item *) malloc(sizeof(struct list_item));
	
	if (li == NULL)
	{
		return ENOMEM;
	}
	
	li->next = NULL;
	li->prev = NULL;
	li->data = data;
	
	if (ll->head == NULL)
	{
		ll->head = ll->tail = li;
	}
	else
	{
		ll->tail->next = li;
		li->prev = ll->tail;
		ll->tail = li;
	}
	
	ll->size++;
	
	return 0;
}

struct list * list_join(struct list * ll_base, struct list * ll_add)
{
	if (ll_base->size > 0 && ll_add->size > 0)
	{
		ll_base->tail->next = ll_add->head;
		ll_add->head->prev = ll_base->tail;
		
		ll_base->tail = ll_add->tail;
		ll_base->size += ll_add->size;
		
		ll_add->size		= 0;
		ll_add->head		= NULL;
		ll_add->tail		= NULL;
		ll_add->cur			= NULL;
		ll_add->cur_index	= 0;
	}
	
	return ll_base;
}

int list_next(struct list * ll, void ** ret)
{
	if (ll->cur == NULL && ll->cur_index == 0)
	{
		ll->cur = ll->head;
	}
	else
	{
		ll->cur = ll->cur->next;
		ll->cur_index++;
	}
	
	if (ll->cur != NULL)
	{
		*ret = ll->cur->data;
		return 1;
	}
	
	// if we've reached this point, cur is NULL, so we are at the end of the list
	
	return 0;
}

int list_remove_current(struct list * ll)
{
	struct list_item * tmp = NULL;
	
	if (ll->cur != NULL)
	{
		if (ll->cur != ll->head)
		{
			ll->cur->prev->next = ll->cur->next;
			tmp = ll->cur->prev;
		}
		else
		{
			ll->head = ll->head->next;
			ll->cur_index = 0;
		}
		
		if (ll->cur != ll->tail)
		{
			ll->cur->next->prev = ll->cur->prev;
		}
		else
		{
			ll->tail = ll->tail->prev;
		}
		
		free(ll->cur);
		ll->cur = tmp;
		ll->size--;
		return 1;
	}
	
	return 0;
}

void list_reset_hard(struct list * ll)
{
	ll->cur_index = 0;
	ll->cur = NULL;
}

void list_reset(struct list * ll)
{
	if (ll->cur == NULL)
	{
		ll->cur_index = 0;
	}
}

void list_destroy(struct list * ll)
{
	struct list_item * next;
	struct list_item * cur;
	
	cur = ll->head;
	
	// destroy each list item
	
	while (cur != NULL)
	{
		next = cur->next;
		free(cur);
		cur = next;
	}
	
	// destroy the list
	free(ll);
}
