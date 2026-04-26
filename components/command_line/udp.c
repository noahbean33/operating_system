#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main()
{
    printf("UDP Message Sender\n");
    int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
    printf("Socket %d\n", socket_id);

    struct sockaddr_in sin;
    sin.sin_family = AF_INET;
    sin.sin_port = htons(12345);
    sin.sin_addr.s_addr = inet_addr("192.168.178.102");

    const char* message = "This is my very first successful UDP project!!!";
    printf("Sizeof message=%ld\n", strlen(message));
    // Send message to phone
    sendto(socket_id, message, strlen(message), 0, (struct sockaddr *)&sin, sizeof(sin));
}
