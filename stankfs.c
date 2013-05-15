#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "ftree.h"
#include "list.h"
#include <fuse.h>

static const char  *file_path      = "/tmp.txt";
static const char   file_content[] = "Hello World!\n";
static const size_t file_size      = sizeof(file_content)/sizeof(char) - 1;

struct ftree * ft;

void * stank_init(struct fuse_conn_info * info)
{
	printf("init\n");
	
	ft = ftree_create();
	ftree_add_node(ft, ft->root, "tmp.txt", 0);
	
	return NULL;
}

static int
stank_getattr(const char *path, struct stat * stbuf)
{
	printf("stank getattr: %s\n", path);
    struct fnode * fn = NULL;
    
    memset(stbuf, 0, sizeof(struct stat));
    
    fn = ftree_search(ft, path);
    
    if (fn == NULL)
    {
    	return -ENOENT;
    }
    
    if (fn->is_dir)
    {
		stbuf->st_mode = S_IFDIR | 0755;
		stbuf->st_nlink = 3;
    }
    else
    {
    	stbuf->st_mode = S_IFREG | S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH;
       // stbuf->st_mode = S_IRWXU | S_IRWXG | S_IRWXO;
        stbuf->st_nlink = 1;
        stbuf->st_size = strlen(fn->data);
    }
    
    return 0;
}

static int
hello_getattr(const char *path, struct stat *stbuf)
{printf("stank getattr: %s\n", path);
    memset(stbuf, 0, sizeof(struct stat));
	struct fnode * fn = NULL;
	fn = ftree_search(ft, path);
	
	//printf("%s\n", fn->name);
    if (strcmp(path, "/") == 0) { /* The root directory of our file system. */
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 3;printf("dir\n");
    } else if (strcmp(path, file_path) == 0) { /* The only file we have. */
        stbuf->st_mode = S_IWUSR;//S_IFREG | 0777;
        stbuf->st_nlink = 1;
        stbuf->st_size = file_size;printf("file\n");
    } else /* We reject everything else. */
    {
    	printf("nonthing - ");
    	//return -ENOENT;
    }

    return 0;
}

static int
stank_open(const char *path, struct fuse_file_info *fi)
{
	printf("stank Open file: %s\n", path);
	struct fnode * fn = NULL;
	fn = ftree_search(ft, path);
	
	if (fn == NULL || fn->is_dir)
	{
		printf("fn is null or is a dir\n");
		return -ENOENT;
	}
	
	if ((fi->flags & O_ACCMODE) != O_RDWR)
	{
		printf("invalid access modes\n");
		//return -EACCES;
	}

    return 0;
}

static int
hello_open(const char *path, struct fuse_file_info *fi)
{
	printf("Open file: %s\n", path);
    if (strcmp(path, "/tmp.txt") != 0) /* We only recognize one file. */
        return -ENOENT;

    if ((fi->flags & O_ACCMODE) != O_RDONLY) /* Only reading allowed. */
        return -EACCES;

    return 0;
}

static int
stank_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
              off_t offset, struct fuse_file_info *fi)
{
	printf("stank readdir: %s\n", path);
	
	struct fnode * dir = ftree_search(ft, path);
	
	if (dir == NULL)
	{
		return -ENOENT;
	}
	
	assert(dir->is_dir);
	
	filler(buf, ".", NULL, 0);
	filler(buf, "..", NULL, 0);
	
	struct fnode * data = NULL;
	
	list_foreach(dir->children, (void **) &data)
	{printf("data->name %s\n", data->name);
		filler(buf, data->name, NULL, 0);
	}
	
	return 0;
}

static int
hello_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
              off_t offset, struct fuse_file_info *fi)
{
	printf("readdir: %s\n", path);
    if (strcmp(path, "/") != 0) /* We only recognize the root directory. */
        return -ENOENT;

    filler(buf, ".", NULL, 0);           /* Current directory (.)  */
    filler(buf, "..", NULL, 0);          /* Parent directory (..)  */
    filler(buf, file_path + 1, NULL, 0); /* The only file we have. */

    return 0;
}

static int
stank_read(const char *path, char *buf, size_t size, off_t offset,
           struct fuse_file_info *fi)
{
	struct fnode * fn = NULL;
	
	printf("stank read: %s\n", path);
	
	fn = ftree_search(ft, path);
	
	if (fn == NULL || fn->is_dir)
	{
		return -ENOENT;
	}
	
	size_t filesize = strlen(fn->data);
	
    if (offset >= filesize) /* Trying to read past the end of file. */
    {
    	return 0;
    }

    if (offset + size > filesize) /* Trim the read to the file size. */
        size = filesize - offset;

    memcpy(buf, fn->data + offset, size); /* Provide the content. */

    return size;
}

static int
stank_write(const char * path, const char * buf, size_t size, off_t offset,
		      struct fuse_file_info * fi)
{
	struct fnode * fn = NULL;
	
	printf("stank write: %s\n", path);
	
	fn = ftree_search(ft, path);
	
	if (fn == NULL || fn->is_dir)
	{
		return -ENOENT;
	}
	
	size_t filesize = strlen(fn->data);
	
    if (offset >= filesize) /* Trying to read past the end of file. */
    {
    	return 0;
    }

    if (offset + size > filesize) /* Trim the read to the file size. */
        size = filesize - offset;

    memcpy(fn->data + offset, buf, size); /* Provide the content. */

    return size;
}

static int
hello_read(const char *path, char *buf, size_t size, off_t offset,
           struct fuse_file_info *fi)
{
	printf("read: %s\n", path);
    if (strcmp(path, file_path) != 0)
        return -ENOENT;

    if (offset >= file_size) /* Trying to read past the end of file. */
        return 0;

    if (offset + size > file_size) /* Trim the read to the file size. */
        size = file_size - offset;

    memcpy(buf, file_content + offset, size); /* Provide the content. */

    return size;
}

static struct fuse_operations hello_filesystem_operations = {
	//.init		= stank_init,
    //.getattr	= hello_getattr, /* To provide size, permissions, etc. */
    //.open		= hello_open,    /* To enforce read-only access.       */
    //.read		= hello_read,    /* To provide file content.           */
    //.readdir	= hello_readdir, /* To provide directory listing.      */
    
    .getattr	= stank_getattr, /* To provide size, permissions, etc. */
    .open		= stank_open,    /* To enforce read-only access.       */
    .read		= stank_read,    /* To provide file content.           */
    .readdir	= stank_readdir, /* To provide directory listing.      */
    .write		= stank_write
};

int
main(int argc, char **argv)
{printf("main\n");
	ft = ftree_create();
	strcpy(ftree_add_node(ft, ft->root, "tmp.txt", 0)->data, "some temporary text fo show\n");
    return fuse_main(argc, argv, &hello_filesystem_operations, NULL);
}
