TARGETS = stankfs ftree list

CC ?= gcc
CFLAGS_OSXFUSE = -D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=26 -I/usr/local/include/osxfuse/fuse
CFLAGS_EXTRA = -Wall -g $(CFLAGS)
LIBS = -losxfuse

.c:
	$(CC) $(CFLAGS_OSXFUSE) $(CFLAGS_EXTRA) -o $@ $< $(LIBS)

all: $(TARGETS)

stankfs: stankfs.c ftree
	gcc -o stankfs ftree list stankfs.c $(CFLAGS_OSXFUSE) $(CFLAGS_EXTRA) $(LIBS)

ftree: ftree.c list
	gcc -o ftree -c ftree.c

list: list.c
	gcc -o list -c list.c

clean:
	rm -f $(TARGETS) *.o
