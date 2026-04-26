# Filesystem in Userspace

A simple in-memory filesystem implementation using FUSE (Filesystem in Userspace) written in C.

## What is this?

This project demonstrates how to build a basic filesystem that runs in userspace rather than as a kernel module. It creates a virtual filesystem stored entirely in RAM that supports:

- Creating and reading files
- Creating directories  
- Writing to files with proper offset handling
- Listing directory contents

## Technical Details

- **Storage**: In-memory arrays (256 files/directories max, 256 bytes per file)
- **FUSE Version**: 3.0
- **Implementation**: `src/fs.c` (~200 lines)

### Implemented FUSE Operations

- `getattr` - Get file/directory attributes and sizes
- `readdir` - List directory contents
- `read` - Read file data with offset support
- `mkdir` - Create directories
- `mknod` - Create files
- `write` - Write data to files with offset support

## Building

Requires FUSE development libraries:

```bash
# Install FUSE (Linux)
sudo apt-get install libfuse-dev

# Build
make build

# Mount the filesystem
./fs -f /path/to/mountpoint

# In another terminal, test it
cd /path/to/mountpoint
mkdir testdir
echo "Hello World" > testfile
cat testfile
ls -la
```