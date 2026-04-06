#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <poll.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

struct BufferAndSize
{
    int Numbytes;
    char *buf;
};

#define PORT 5002
#define SWAP(type, a, b) \
    do                   \
    {                    \
        type temp = a;   \
        a = b;           \
        b = temp;        \
    } while (0)

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

void AddFileDescriptorToPollArray(struct pollfd **arr, int *totalPresentElements, int *ArraySize, int fd, short tobeMoniteredOrperation)
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
    newMoniteredFd.revents = 0;

    (*arr)[*totalPresentElements] = newMoniteredFd;
    (*totalPresentElements)++;
}
void DeleteFdFromPollFdArray(int position, int *arraysize, struct pollfd arr[])
{
    SWAP(struct pollfd, arr[(*arraysize) - 1], arr[position]);
    (*arraysize)--;
}
void Handle_new_connection(struct pollfd arr[], int arraysize, int *totalFdInArray, int listener)
{
    struct sockaddr_storage peerAddr;
    socklen_t peerAddrLen = sizeof(peerAddr);

    int newSocket;
    newSocket = accept(listener, (struct sockaddr *)&peerAddr, &peerAddrLen);

    if (newSocket == -1)
    {
        perror("Couldn't accept the connection: ");
        return;
    }

    AddFileDescriptorToPollArray(&arr, totalFdInArray, arraysize, newSocket, POLLIN);
}

struct BufferAndSize handle_Client_data(int listener, struct pollfd array[], int *arraysize, int fdi)
{
    struct BufferAndSize BuffAndSize;
    int bufferLength = 256;
    char buf[bufferLength];
    memset(buf, 0, bufferLength);
    if (array[fdi].revents & POLLIN)
    {
        int nbytes = recv(array[fdi].fd, buf, bufferLength, 0);
        if (nbytes == -1)
        {
            fprintf(stderr, "Error while receiving the data from the socket: %d", array[fdi].fd);
            perror("Error: ");
        }
        else
        {
            BuffAndSize.Numbytes = nbytes;
            BuffAndSize.buf = buf;
            close(array[fdi].fd);
            DeleteFdFromPollFdArray(fdi, arraysize, array);
        }
    }

    return BuffAndSize;
}

void SendDataToAllSockets(struct pollfd array[], int *arraysize, int listener, char *buffer, int buffersize, int sender)
{
    for (int i = 0; i < arraysize; i++)
    {
        if (array[i].fd != listener && array[i].fd != sender)
        {
            if (array[i].revents & POLLOUT)
            {
                int bytessend = send(array[i].fd, buffer, buffersize, 0);
                if (bytessend == -1)
                {
                    fprintf(stderr, "error while sending the data to socket: %d", array[i].fd);
                    perror("Error: ");
                }
                else
                {
                    close(array[i].fd);
                    DeleteFdFromPollFdArray(i, arraysize, array);
                }
            }
        }
    }
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

    int activeevents = poll(array, totalElementsinArray, 5000);
    if (activeevents == -1)
    {
        perror("Poll Error: ");
        return EXIT_FAILURE;
    }

    while (pollfdArraySize > 0)
    {
        Handle_new_connection(array, pollfdArraySize, &totalElementsinArray, MainOpenedSocket);
        for (int i = 0; i < pollfdArraySize; i++)
        {
            struct BufferAndSize buffAndSize = handle_Client_data(MainOpenedSocket, array, &pollfdArraySize, i);

            SendDataToAllSockets(array, &pollfdArraySize, MainOpenedSocket, buffAndSize.buf, buffAndSize.Numbytes, array[i].fd);
        }
    }

    return 0;
}