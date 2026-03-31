#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define  BUFF_SIZE 128

int main(void)
{
    char MainBuffer[BUFF_SIZE];

    struct sockaddr_storage peerAddr;
    int peerAddrLength = sizeof(peerAddr);

    struct addrinfo hints, *res = NULL, *temp = NULL;
    int MainSocketFd;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(NULL, "8080", &hints, &res);

    if (status != 0)
    {
        fprintf(stderr, "Coudn't get the addresses to initialize the sockets. Error: %s", gai_strerror(status));
        return 1;
    }

    for (temp = res; temp != NULL; temp = temp->ai_next)
    {
        MainSocketFd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

        if (MainSocketFd == -1)
            continue;

        if (bind(MainSocketFd, temp->ai_addr, temp->ai_addrlen) == 0)
            break;

        close(MainSocketFd);
    }

    freeaddrinfo(res);

    if (listen(MainSocketFd, 5) == -1)
    {

        perror("Listening error");
        close(MainSocketFd);
        return 1;
    }
    fprintf(stdout, "Started Listening to the port 8080......\n");

    int CurrentLyConnectedFd;

    CurrentLyConnectedFd = accept(MainSocketFd, (struct sockaddr *)&peerAddr, &peerAddrLength);

    if (CurrentLyConnectedFd == -1)
        perror("error occured while accepting the socket connection.");

    int BytesReceived = recv(CurrentLyConnectedFd, MainBuffer, 128, 0);
    if (BytesReceived == -1)
        perror("error occured while receiving the data from connected socket.");
    else if (BytesReceived == 0)
    {
        printf("Can't Revcieve Message Anymore Socket is shut down.");
    }

    int BytesSend = send(CurrentLyConnectedFd, MainBuffer, BytesReceived, 0);
    if (BytesSend == -1)
        perror("error occured while sending the data to the connected socket.");

    write(2, "Received Data Sent back.\n", strlen("Received Data Sent back.\n "));
    shutdown(CurrentLyConnectedFd, SHUT_RDWR);
    shutdown(MainSocketFd, SHUT_RDWR);
    close(CurrentLyConnectedFd);
    close(MainSocketFd);
    return 0;
}
