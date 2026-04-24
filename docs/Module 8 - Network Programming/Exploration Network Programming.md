# Exploration: Network Programming

## Introduction

IPC over a network typically follows a **Client/Server architecture**. A server process runs at a machine with a well-known address, waiting for service requests from client processes on other machines.

### Client-Server Interaction

1. Client sends a request to the server
2. Server receives the request
3. Server performs the service
4. Server sends a response back to the client
5. Client receives and processes the response

For this interaction to succeed, both sides follow a common **protocol** (e.g., TCP/IP).

---

## Network Layer Model (TCP/IP)

| Layer | Common Protocols |
|-------|-----------------|
| **Application** | HTTP, SFTP, DNS |
| **Transport** | TCP, UDP |
| **Network** | IP |
| **Link** | Ethernet, 802.11 |
| **Physical** | Twisted pair copper, fiber |

### Transport Layer Protocols

**TCP (Transmission Control Protocol):**
- Connection-oriented, byte-stream interface
- Guarantees packets arrive in order
- Used by HTTP

**UDP (Universal Datagram Protocol):**
- Connectionless, no delivery guarantee
- Lower overhead, faster transfer
- Used by DNS

---

## Programmer's View of the Internet

### IP Addresses

```c
#include <netinet/in.h>

struct in_addr {
   unsigned int s_addr;
};
```

**Dotted-decimal notation** example: `35.161.98.54`

Conversion functions:
- `inet_ntoa()` — IP address → dotted-decimal string
- `inet_aton()` — dotted-decimal string → IP address

### Internet Domain Names

Domain names (e.g., `oregonstate.edu`) map to IP addresses via **DNS** (Domain Name System).

```c
#include <netdb.h>

struct hostent {
    char  *h_name;       /* official name of host */
    char **h_aliases;    /* array of alternate names */
    int    h_addrtype;   /* AF_INET or AF_INET6 */
    int    h_length;     /* length of address */
    char **h_addr_list;  /* array of addresses */
};
```

- `gethostbyname()` — look up by domain name
- `gethostbyaddr()` — look up by IP address

### Example: DNS Lookup

```c
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr,"USAGE: %s $hostname\n", argv[0]);
    exit(0);
  }
  struct hostent *hostptr = gethostbyname(argv[1]);
  if(hostptr == NULL){ printf("Couldn't locate %s\n", argv[1]); exit(0); }

  printf("Official domain name: %s\n", hostptr->h_name);

  int i = 0;
  printf("Aliases:\n");
  while(hostptr->h_aliases[i] != NULL){ printf("\t%s\n", hostptr->h_aliases[i]); i++; }

  int j = 0;
  printf("Addresses:\n");
  struct in_addr addr;
  while(hostptr->h_addr_list[j] != NULL){
    addr.s_addr = *(unsigned int *) hostptr->h_addr_list[j];
    printf("\t%s\n", inet_ntoa(addr));
    j++;
  }
  return 0;
}
```

---

## Network Byte Order vs. Host Byte Order

TCP/IP uses **big-endian** (network byte order). Conversion functions:

| Function | Description |
|----------|-------------|
| `htonl()` | Host → network (unsigned long) |
| `htons()` | Host → network (unsigned short) |
| `ntohl()` | Network → host (unsigned long) |
| `ntohs()` | Network → host (unsigned short) |

---

## Additional Resources

- Chapter 58 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
