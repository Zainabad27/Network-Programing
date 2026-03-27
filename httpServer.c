#define _POSIX_C_SOURCE 200112L
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>
#include <sys/types.h>
#include <netdb.h>
#include <string.h>
#include<unistd.h>

int main()
{
    struct addrinfo hints;

    struct addrinfo *result, *temp;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int res = getaddrinfo(NULL, "8080", &hints, &result);
    if (res != 0)
    {
        fprintf(stderr,"error while getting the address info from getaddrinfo: %s",gai_strerror((res)));
        return 1;
    }

    for (temp = result; temp != NULL; temp = temp->ai_next)
    {
        int sfd = socket(temp->ai_family, temp->ai_socktype, temp->ai_protocol);

        if (sfd == -1)
            continue;

        if (bind(sfd, temp->ai_addr, temp->ai_addrlen) == 0)
            break;

        close(sfd);
    }
    freeaddrinfo(result);
    freeaddrinfo(temp);
}