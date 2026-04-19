# Exploration: Client-Server Communication Via Sockets

## Introduction

To completely open a socket for reading and writing, additional system calls are needed beyond `socket()`. The calls differ for **client** vs. **server** programs.

### Overview of Client-Server Socket Communication

1. **Server:** creates a listening socket → `bind()` → `listen()` → loops with `accept()`
2. **Client:** creates a socket → `connect()` to server
3. Both use `send()` / `recv()` to exchange data
4. Close sockets when done

---

## Server Program

### Steps

1. Create socket with `socket()`
2. Set up `sockaddr_in` with port number
3. `bind()` — associate socket with port
4. `listen()` — start listening for connections
5. Loop: `accept()` connections → `recv()` / `send()` → `close()` connected socket

### Server API

#### `bind()`

```c
int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Associates the socket with an address/port. Returns 0 on success, -1 on error.

#### `listen()`

```c
int listen(int sockfd, int backlog);
```

Marks the socket as a listening socket. `backlog` = max pending connection requests.

#### `accept()`

```c
int accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

Blocks until a connection request arrives. Creates and returns a **new connected socket** for the client. The original listening socket remains unaffected.

#### `recv()`

```c
ssize_t recv(int sockfd, void *buf, size_t len, int flags);
```

Reads up to `len` bytes from the socket. Blocks if no data available. May need **multiple calls** to read the complete message.

#### `send()`

```c
ssize_t send(int sockfd, const void *buf, size_t len, int flags);
```

Sends `len` bytes over the socket. May send **fewer bytes** than requested — handle partial sends with a loop.

### Example Server

```c
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) { perror(msg); exit(1); }

void setupAddressStruct(struct sockaddr_in* address, int portNumber){
  memset((char*) address, '\0', sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(portNumber);
  address->sin_addr.s_addr = INADDR_ANY;
}

int main(int argc, char *argv[]){
  int connectionSocket, charsRead;
  char buffer[256];
  struct sockaddr_in serverAddress, clientAddress;
  socklen_t sizeOfClientInfo = sizeof(clientAddress);

  if (argc < 2) { fprintf(stderr,"USAGE: %s port\n", argv[0]); exit(1); }

  int listenSocket = socket(AF_INET, SOCK_STREAM, 0);
  if (listenSocket < 0) error("ERROR opening socket");

  setupAddressStruct(&serverAddress, atoi(argv[1]));

  if (bind(listenSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0)
    error("ERROR on binding");

  listen(listenSocket, 5);

  while(1){
    connectionSocket = accept(listenSocket, (struct sockaddr *)&clientAddress, &sizeOfClientInfo);
    if (connectionSocket < 0) error("ERROR on accept");

    memset(buffer, '\0', 256);
    charsRead = recv(connectionSocket, buffer, 255, 0);
    if (charsRead < 0) error("ERROR reading from socket");
    printf("SERVER: I received this from the client: \"%s\"\n", buffer);

    charsRead = send(connectionSocket, "I am the server, and I got your message", 39, 0);
    if (charsRead < 0) error("ERROR writing to socket");

    close(connectionSocket);
  }
  close(listenSocket);
  return 0;
}
```

---

## Client Program

### Steps

1. Create socket with `socket()`
2. Set up `sockaddr_in` with server hostname and port
3. `connect()` to server
4. `send()` / `recv()` data
5. `close()` socket

#### `connect()`

```c
int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

Connects the client socket to the server. Returns 0 on success, -1 on error.

### Example Client

```c
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg) { perror(msg); exit(0); }

void setupAddressStruct(struct sockaddr_in* address, int portNumber, char* hostname){
  memset((char*) address, '\0', sizeof(*address));
  address->sin_family = AF_INET;
  address->sin_port = htons(portNumber);
  struct hostent* hostInfo = gethostbyname(hostname);
  if (hostInfo == NULL) { fprintf(stderr, "CLIENT: ERROR, no such host\n"); exit(0); }
  memcpy((char*) &address->sin_addr.s_addr, hostInfo->h_addr_list[0], hostInfo->h_length);
}

int main(int argc, char *argv[]) {
  int socketFD, charsWritten, charsRead;
  struct sockaddr_in serverAddress;
  char buffer[256];

  if (argc < 3) { fprintf(stderr,"USAGE: %s hostname port\n", argv[0]); exit(0); }

  socketFD = socket(AF_INET, SOCK_STREAM, 0);
  if (socketFD < 0) error("CLIENT: ERROR opening socket");

  setupAddressStruct(&serverAddress, atoi(argv[2]), argv[1]);

  if (connect(socketFD, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) < 0)
    error("CLIENT: ERROR connecting");

  printf("CLIENT: Enter text to send to the server, and then hit enter: ");
  memset(buffer, '\0', sizeof(buffer));
  fgets(buffer, sizeof(buffer) - 1, stdin);
  buffer[strcspn(buffer, "\n")] = '\0';

  charsWritten = send(socketFD, buffer, strlen(buffer), 0);
  if (charsWritten < 0) error("CLIENT: ERROR writing to socket");

  memset(buffer, '\0', sizeof(buffer));
  charsRead = recv(socketFD, buffer, sizeof(buffer) - 1, 0);
  if (charsRead < 0) error("CLIENT: ERROR reading from socket");
  printf("CLIENT: I received this from the server: \"%s\"\n", buffer);

  close(socketFD);
  return 0;
}
```

---

## Additional Resources

- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- Chapter 59 of *The Linux Programming Interface* by Michael Kerrisk (No Starch Press, 2010)
