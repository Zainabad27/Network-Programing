#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int main()
{
    int Socketfd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[128];

    if (Socketfd < 0)
    {
        printf("Hello");
    }
    else
    {
        printf("hello from %s\nthe socket fd is: %i\n", "zain", Socketfd);
    }

    struct sockaddr *socketAddress;
    struct sockaddr_in *my_Addr;
    my_Addr->sin_family = AF_INET;
    my_Addr->sin_port = htons(2020);
    my_Addr->sin_addr.s_addr = htons("192.168.1.253");
    {
        /* data */
    };

    socketAddress->sa_family = AF_INET;
    // socketAddress->sa_data=buffer;

    bind(Socketfd, (struct sockaddr *)&my_Addr, sizeof(my_Addr));


    
}