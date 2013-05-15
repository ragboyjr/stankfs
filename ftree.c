//
//  ftree.c
//  link_list
//
//  Created by RJ Garcia on 5/14/13.
//  Copyright (c) 2013 RJ Garcia. All rights reserved.
//

#include "ftree.h"
#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// private function for deleting the nodes
struct fnode * fnode_create();
void fnode_destroy(struct fnode *);

struct ftree * ftree_create()
{
	struct ftree * ft = malloc(sizeof(struct ftree));
	ft->size = 0;
	ft->root = fnode_create("/", 1);
	
	return ft;
}
struct fnode * ftree_add_node(struct ftree * this, struct fnode * parent, const char * name, int is_dir)
{
	struct fnode * fn = fnode_create(name, is_dir);

	fn->parent = parent;
	list_add(parent->children, (void *) fn);
	
	this->size++;
	return fn;
}

void _ftree_delete_node(struct ftree * this, struct fnode * fn)
{
	struct fnode * data = NULL;
	
	list_foreach(fn->children, (void **) &data)
	{
		if (data->is_dir)
		{
			_ftree_delete_node(this, data);
		}
		
		list_remove_current(fn->children);

		fnode_destroy(data);
		this->size--;
	}
}

void ftree_delete_node(struct ftree * this, struct fnode * fn)
{
	struct fnode * data;
	
	// we have a recursive delete
	if (fn->is_dir)
	{
		_ftree_delete_node(this, fn);
		
		assert(fn->children->size == 0);
	}
	
	// at this point we know that fn has no children
	
	if (fn->parent)
	{
		list_foreach(fn->parent->children, (void **) &data)
		{
			if (data == fn)
			{
				// remove myself from the parents list
				list_remove_current(fn->parent->children);
				break;
			}
		}
	}
	else // we are deleting root
	{
		this->root = NULL;	// set to null to know that the tree is empty
	}

	fnode_destroy(fn);
	this->size--;
}

struct fnode * ftree_search(struct ftree * this, const char * name)
{
	struct fnode * fn = NULL;
	struct fnode * data = NULL;
	int match = 0;
	char * path = strdup(name);
	unsigned long len = strlen(path);
	char * tok = NULL;
	
	if (this->root == NULL)
	{
		return fn;
	}
	
	assert(len);

	if (path[0] != '/')
	{
		return fn;
	}
	
	// let's check for the special case of reading the root dir
	if (len == 1)
	{
		fn = this->root;
		return fn;
	}
	
	tok = strtok(path, "/");
	fn = this->root;
	
	while (tok)
	{
		match = 0;
		list_foreach(fn->children, (void **) &data)
		{
			if (strcmp(data->name, tok) == 0)
			{
				fn = data;
				match = 1;
				break;
			}
		}
		
		if (match == 0)
		{
			break;
		}
		
		tok = strtok(NULL, "/");
	}
	
	if (match == 0)
	{
		return NULL;
	}
	
	return fn;
}

void ftree_destroy(struct ftree * this)
{
	ftree_delete_node(this, this->root);
	free(this);
}

void _ftree_print(struct fnode * fn, const char * prefix)
{
	struct fnode * data = NULL;
	char str[80];
	strcpy(str, "");
	
	list_foreach(fn->children, (void **) &data)
	{
		printf("%s%s/%s\n", prefix, fn->name, data->name);
		
		if (data->is_dir)
		{
			_ftree_print(data, strcat(strcat(strcat(str, prefix), fn->name), "/"));
		}
	}
}

void ftree_print(struct ftree * this)
{
	_ftree_print(this->root, "");
}


/* F N O D E   F U N C T I O N S */

struct fnode * fnode_create(const char * name, int is_dir)
{
	struct fnode * fn = malloc(sizeof(struct fnode));
	fn->name = strdup(name);
	fn->is_dir = is_dir;
	
	if (is_dir)
	{
		fn->children = list_create();
	}
	else
	{
		fn->data = malloc(FNODE_BLOCK_SIZE);
	}
	
	fn->parent = NULL;
	
	return fn;
}

void fnode_destroy(struct fnode * fn)
{
	// delete the node
	free(fn->name);
	
	if (fn->is_dir)
	{
		list_destroy(fn->children);
	}
	
	free(fn->data);
	free(fn);
}
