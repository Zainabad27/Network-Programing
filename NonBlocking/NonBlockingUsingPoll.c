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

int GetASocket(void)
{
    int MainSocket = -1;
    struct addrinfo hints, *resultAddresshead, *temp; // resultaddresshead cuz getaddrinfo() returns a linked list.
    memset(&hints, 0, sizeof(hints));

    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    int returnedVal = getaddrinfo(NULL, PORT, &hints, resultAddresshead);
    if (returnedVal != 0)
    {
        fprintf(stdout,"Couldn't get the address to initialize the socket. %s  \n", gai_strerror(returnedVal));

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


    if(temp==NULL){
        fprintf(stdout,"Couldn't Initialize the socket.");
        freeaddrinfo(resultAddresshead);
        return -1;
    }
}
int main()
{

    return 0;
}