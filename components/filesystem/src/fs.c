/**
 * Simple In-Memory Filesystem using FUSE
 * 
 * This filesystem stores all data in RAM using fixed-size arrays.
 * It supports basic operations: creating files/directories, reading, and writing.
 * All data is lost when the filesystem is unmounted.
 * 
 * Limitations:
 * - Maximum 256 files and 256 directories
 * - Maximum 256 bytes per file
 * - Flat directory structure (all items in root)
 * - No delete operations
 */

#define FUSE_USE_VERSION 30

#include <fuse.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

/* ========== Data Structures ========== */

/* Directory storage: up to 256 directories, each name up to 255 chars */
char dir_list[ 256 ][ 256 ];
int curr_dir_idx = -1;  /* Index of last directory (-1 means empty) */

/* File name storage: up to 256 files, each name up to 255 chars */
char files_list[ 256 ][ 256 ];
int curr_file_idx = -1;  /* Index of last file (-1 means empty) */

/* File content storage: up to 256 files, each content up to 255 bytes */
char files_content[ 256 ][ 256 ];
int curr_file_content_idx = -1;  /* Index of last file content */

/* ========== Helper Functions ========== */

/**
 * Add a new directory to the filesystem
 * @param dir_name Name of the directory (without leading '/')
 */
void add_dir( const char *dir_name )
{
	/* Check if we've reached the maximum number of directories */
	if ( curr_dir_idx >= 255 )
		return;
	
	curr_dir_idx++;
	/* Use strncpy to prevent buffer overflow */
	strncpy( dir_list[ curr_dir_idx ], dir_name, 255 );
	dir_list[ curr_dir_idx ][ 255 ] = '\0';  /* Ensure null termination */
}

/**
 * Check if a path is a directory
 * @param path Full path (e.g., "/dirname")
 * @return 1 if directory exists, 0 otherwise
 */
int is_dir( const char *path )
{
	path++;  /* Skip the leading '/' character */
	
	/* Search through all directories for a match */
	for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
		if ( strcmp( path, dir_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

/**
 * Add a new file to the filesystem
 * @param filename Name of the file (without leading '/')
 */
void add_file( const char *filename )
{
	/* Check if we've reached the maximum number of files */
	if ( curr_file_idx >= 255 )
		return;
	
	curr_file_idx++;
	/* Use strncpy to prevent buffer overflow */
	strncpy( files_list[ curr_file_idx ], filename, 255 );
	files_list[ curr_file_idx ][ 255 ] = '\0';  /* Ensure null termination */
	
	/* Initialize empty content for the new file */
	curr_file_content_idx++;
	files_content[ curr_file_content_idx ][ 0 ] = '\0';
}

/**
 * Check if a path is a file
 * @param path Full path (e.g., "/filename")
 * @return 1 if file exists, 0 otherwise
 */
int is_file( const char *path )
{
	path++;  /* Skip the leading '/' character */
	
	/* Search through all files for a match */
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return 1;
	
	return 0;
}

/**
 * Get the array index of a file
 * @param path Full path (e.g., "/filename")
 * @return Index in files_list array, or -1 if not found
 */
int get_file_index( const char *path )
{
	path++;  /* Skip the leading '/' character */
	
	/* Search through all files and return the index if found */
	for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
		if ( strcmp( path, files_list[ curr_idx ] ) == 0 )
			return curr_idx;
	
	return -1;  /* File not found */
}

/**
 * Write content to a file (simple overwrite, used internally)
 * @param path Full path (e.g., "/filename")
 * @param new_content Content to write
 */
void write_to_file( const char *path, const char *new_content )
{
	int file_idx = get_file_index( path );
	
	if ( file_idx == -1 )  /* File not found */
		return;
		
	/* Use strncpy to prevent buffer overflow */
	strncpy( files_content[ file_idx ], new_content, 255 ); 
	files_content[ file_idx ][ 255 ] = '\0';  /* Ensure null termination */
}

/* ========== FUSE Callback Functions ========== */

/**
 * Get file/directory attributes (called by stat, ls -l, etc.)
 * @param path Path to the file/directory
 * @param st Stat structure to fill with attributes
 * @return 0 on success, -ENOENT if path doesn't exist
 */
static int do_getattr( const char *path, struct stat *st )
{
	/* Set ownership to the user who mounted the filesystem */
	st->st_uid = getuid();
	st->st_gid = getgid();
	
	/* Set timestamps to current time */
	st->st_atime = time( NULL );  /* Last access time */
	st->st_mtime = time( NULL );  /* Last modification time */
	
	/* Check if path is root directory or a created directory */
	if ( strcmp( path, "/" ) == 0 || is_dir( path ) == 1 )
	{
		st->st_mode = S_IFDIR | 0755;  /* Directory with rwxr-xr-x permissions */
		st->st_nlink = 2;  /* Every directory has 2 hard links (. and ..) */
	}
	/* Check if path is a file */
	else if ( is_file( path ) == 1 )
	{
		st->st_mode = S_IFREG | 0644;  /* Regular file with rw-r--r-- permissions */
		st->st_nlink = 1;  /* Regular files have 1 hard link */
		
		/* Get actual file size from content */
		int file_idx = get_file_index( path );
		if ( file_idx != -1 )
			st->st_size = strlen( files_content[ file_idx ] );
		else
			st->st_size = 0;
	}
	else
	{
		/* Path doesn't exist */
		return -ENOENT;
	}
	
	return 0;
}

/**
 * Read directory contents (called by ls)
 * @param path Path to the directory
 * @param buffer Buffer to fill with directory entries
 * @param filler Function to add entries to the buffer
 * @param offset Offset (unused in this implementation)
 * @param fi File info (unused in this implementation)
 * @return 0 on success
 */
static int do_readdir( const char *path, void *buffer, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi )
{
	/* Add standard directory entries */
	filler( buffer, ".", NULL, 0 );   /* Current directory */
	filler( buffer, "..", NULL, 0 );  /* Parent directory */
	
	/* Only root directory is supported (flat structure) */
	if ( strcmp( path, "/" ) == 0 )
	{
		/* Add all directories */
		for ( int curr_idx = 0; curr_idx <= curr_dir_idx; curr_idx++ )
			filler( buffer, dir_list[ curr_idx ], NULL, 0 );
	
		/* Add all files */
		for ( int curr_idx = 0; curr_idx <= curr_file_idx; curr_idx++ )
			filler( buffer, files_list[ curr_idx ], NULL, 0 );
	}
	
	return 0;
}

/**
 * Read data from a file (called by cat, read(), etc.)
 * @param path Path to the file
 * @param buffer Buffer to fill with file data
 * @param size Number of bytes to read
 * @param offset Starting position in the file
 * @param fi File info (unused in this implementation)
 * @return Number of bytes read, or -ENOENT if file doesn't exist
 */
static int do_read( const char *path, char *buffer, size_t size, off_t offset, struct fuse_file_info *fi )
{
	int file_idx = get_file_index( path );
	
	/* Check if file exists */
	if ( file_idx == -1 )
		return -ENOENT;
	
	char *content = files_content[ file_idx ];
	int len = strlen( content );
	
	/* If offset is beyond file length, return 0 (EOF) */
	if ( offset >= len )
		return 0;
	
	/* Calculate how many bytes to actually read */
	int bytes_to_read = len - offset;
	if ( bytes_to_read > size )
		bytes_to_read = size;  /* Don't read more than requested */
	
	/* Copy data to buffer */
	memcpy( buffer, content + offset, bytes_to_read );
		
	return bytes_to_read;  /* Return number of bytes read */
}

/**
 * Create a new directory (called by mkdir)
 * @param path Path for the new directory
 * @param mode Permissions mode (unused - we use 0755)
 * @return 0 on success
 */
static int do_mkdir( const char *path, mode_t mode )
{
	path++;  /* Skip the leading '/' */
	add_dir( path );
	
	return 0;
}

/**
 * Create a new file (called by touch, creat(), etc.)
 * @param path Path for the new file
 * @param mode Permissions mode (unused - we use 0644)
 * @param rdev Device number (unused for regular files)
 * @return 0 on success
 */
static int do_mknod( const char *path, mode_t mode, dev_t rdev )
{
	path++;  /* Skip the leading '/' */
	add_file( path );
	
	return 0;
}

/**
 * Write data to a file (called by write(), echo >, etc.)
 * @param path Path to the file
 * @param buffer Data to write
 * @param size Number of bytes to write
 * @param offset Starting position in the file
 * @param info File info (unused in this implementation)
 * @return Number of bytes written, or -ENOENT if file doesn't exist
 */
static int do_write( const char *path, const char *buffer, size_t size, off_t offset, struct fuse_file_info *info )
{
	int file_idx = get_file_index( path );
	
	/* Check if file exists */
	if ( file_idx == -1 )
		return -ENOENT;
	
	int current_len = strlen( files_content[ file_idx ] );
	int new_len = offset + size;
	
	/* Enforce 255 byte file size limit */
	if ( new_len > 255 )
		new_len = 255;
	
	/* If writing beyond current file length, pad with zeros */
	if ( offset > current_len )
	{
		memset( files_content[ file_idx ] + current_len, 0, offset - current_len );
	}
	
	/* Calculate actual bytes to write */
	int bytes_to_write = new_len - offset;
	if ( bytes_to_write > 0 )
	{
		/* Copy data to file content at specified offset */
		memcpy( files_content[ file_idx ] + offset, buffer, bytes_to_write );
		files_content[ file_idx ][ new_len ] = '\0';  /* Null terminate */
	}
	
	return bytes_to_write > 0 ? bytes_to_write : 0;
}

/**
 * FUSE operations structure
 * Maps FUSE callbacks to our implementation functions
 */
static struct fuse_operations operations = {
    .getattr	= do_getattr,   /* Get file attributes */
    .readdir	= do_readdir,   /* Read directory contents */
    .read		= do_read,       /* Read file data */
    .mkdir		= do_mkdir,      /* Create directory */
    .mknod		= do_mknod,      /* Create file */
    .write		= do_write,      /* Write file data */
};

/**
 * Main entry point
 * Initializes FUSE with our operations and starts the filesystem
 */
int main( int argc, char *argv[] )
{
	return fuse_main( argc, argv, &operations, NULL );
}
