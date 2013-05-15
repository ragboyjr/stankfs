//
//  ftree.h
//  link_list
//
//  Created by RJ Garcia on 5/14/13.
//  Copyright (c) 2013 RJ Garcia. All rights reserved.
//

#ifndef link_list_ftree_h
#define link_list_ftree_h

#define FNODE_BLOCK_SIZE 1024

struct list;

struct fnode {
	char * name;
	int is_dir;
	char * data;
	struct list * children;
	struct fnode * parent;
};

struct ftree {
	struct fnode * root;
	int size;
};

struct ftree * ftree_create();
struct fnode * ftree_add_node(struct ftree *, struct fnode *, const char *, int);
void ftree_delete_node(struct ftree *, struct fnode *);
struct fnode * ftree_search(struct ftree *, const char *);
void ftree_print(struct ftree *);
void ftree_destroy(struct ftree *);

#endif
