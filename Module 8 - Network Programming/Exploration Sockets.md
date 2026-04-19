# Exploration: Sockets

## Introduction

Linux provides the **sockets API** for writing applications that communicate over the network. A socket is the endpoint of a communication link between two processes. Sockets can be used for communication on the same machine or across different machines.

---

## Creating a Socket

```c
#include <sys/socket.h>
#include <sys/types.h>

int socket(int domain, int type, int protocol);
```

| Argument | Value | Description |
|----------|-------|-------------|
| `domain` | `AF_INET` | IPv4 protocol suite |
| `type` | `SOCK_STREAM` | TCP socket |
| `type` | `SOCK_DGRAM` | UDP socket |
| `protocol` | `0` | Default protocol |

Returns a **socket descriptor** (file descriptor) on success, `-1` on error. The socket descriptor is added to the process's open file descriptor table alongside stdin, stdout, stderr, and any open files.

> At this point, the socket is **not yet ready** for reading/writing — additional calls are needed.

---

## Socket Addresses

An IP address alone is insufficient to identify a process — a **port number** (16-bit unsigned integer, 0–65,535) is also needed.

| Port Range | Name | Description |
|-----------|------|-------------|
| 0–1023 | Well-known / system ports | Reserved (HTTP: 80, SSH: 22) |
| 49152–65535 | Ephemeral / dynamic ports | Cannot be reserved; assigned by OS |

### `sockaddr_in` Structure

```c
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

struct sockaddr_in {
  sa_family_t sin_family;  /* Address family: AF_INET */
  in_port_t sin_port;      /* Port number in network byte order */
  struct in_addr sin_addr; /* IP address */
};
```

> Values are stored in **network byte order** — use `htons()`, `ntohs()`, etc. for conversion.

### Example: Setting Up a Socket Address

```c
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc < 3) {
    fprintf(stderr,"USAGE: %s hostname port\n", argv[0]);
    exit(0);
  }

  struct sockaddr_in address;
  memset((char*) &address, '\0', sizeof(address));
  address.sin_family = AF_INET;
  address.sin_port = htons(atoi(argv[2]));

  struct hostent* hostInfo = gethostbyname(argv[1]);
  if (hostInfo == NULL) { fprintf(stderr, "ERROR, no such host\n"); exit(0); }
  memcpy(&address.sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);

  printf("IP address for %s: %s\n", argv[1], inet_ntoa(address.sin_addr));
  return 0;
}
```

---

## Additional Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- Chapter 59 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
