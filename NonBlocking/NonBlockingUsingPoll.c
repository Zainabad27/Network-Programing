#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define PORT 5002

void *GetAddress(struct addrinfo *address)
{
    if (address->ai_family == AF_INET6)
    {
        return (struct sockaddr_in *)&address->ai_addr;
    }
    else if (address->ai_family == AF_INET6)
    {

        return (struct sockaddr_in6 *)&address->ai_addr;
    }

    return NULL;
}

int GetASocket(void) // returns file descriptor. -1 incase of error
{
    int MainSocket = -1;
    struct addrinfo hints, *resultAddresshead, *temp; // resultaddresshead cuz getaddrinfo() returns a linked list.
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int returnedVal = getaddrinfo(NULL, PORT, &hints, resultAddresshead);
    if (returnedVal != 0)
    {
        fprintf(stdout, "Couldn't get the address to initialize the socket. %s  \n", gai_strerror(returnedVal));

        return EXIT_FAILURE;
    }

    for (temp = resultAddresshead; temp != NULL; temp = temp->ai_next)
    {
        MainSocket = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);
        if (MainSocket == -1)
            continue;

        // coudt set the options for the socket.

        if (bind(MainSocket, (struct sockaddr *)GetAddress(temp->ai_addr), sizeof(struct sockaddr)) == 0)

            break;

        close(MainSocket);
    }

    if (temp == NULL)
    {
        fprintf(stdout, "Couldn't Initialize the socket.");
        freeaddrinfo(resultAddresshead);
        return -1;
    }

    return MainSocket;
}

void AddFileDescriptorToPollArray(struct pollfd **arr, int *totalPresentElements, int *ArraySize, int fd, int tobeMoniteredOrperation)
{
    struct pollfd *temp;
    if (*totalPresentElements == *ArraySize)
    {
        temp = realloc(*arr, ((*ArraySize) * 2) * sizeof((**arr)));
        if (temp == NULL)
        {
            fprintf(stderr, "Couldn't increase the size of the array To add a struct pollfd file descriptor.\n");

            return;
        }

        *arr = temp;
        *ArraySize = 2 * (*ArraySize);
    }

    struct pollfd newMoniteredFd;
    newMoniteredFd.fd = fd;
    newMoniteredFd.events = tobeMoniteredOrperation;

    (*arr)[*totalPresentElements] = newMoniteredFd;
    (*totalPresentElements)++;
}

int main()
{
    int pollfdArraySize = 10;
    int totalElementsinArray = 0;
    int MainOpenedSocket = GetASocket();
    if (MainOpenedSocket == -1)
        return EXIT_FAILURE;

    struct pollfd array[pollfdArraySize];
    AddFileDescriptorToPollArray(&array, &totalElementsinArray, &pollfdArraySize, MainOpenedSocket, POLLIN);

    if (listen(array[0].fd, 10) == -1)
    {
        fprintf(stderr, "The Server couldn't start.");
        return EXIT_FAILURE;
    }


    poll(array,totalElementsinArray,5000);

    return 0;
}