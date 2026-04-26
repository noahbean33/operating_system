COMPILER = gcc
FILESYSTEM_FILES = src/fs.c

build: $(FILESYSTEM_FILES)
	$(COMPILER) $(FILESYSTEM_FILES) -o lsysfs `pkg-config fuse --cflags --libs`
	echo 'To Mount: ./lsysfs -f [mount point]'

clean:
	rm src/fs
