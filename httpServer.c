#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <errno.h>

int main()
{
    int Socketfd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[128];
    char peerIp[128];
    struct sockaddr_in *my_Addr;
    struct sockaddr_storage *peer_Addr;
    
    if (Socketfd < 0)
    {
        dprintf(2, "Socket did not get initialized at line number %i", (__LINE__ - 5));
    }
    else
    {
        printf("Socket Got initialized with File descriptor %i", Socketfd);
    }

    my_Addr->sin_family = AF_INET;
    my_Addr->sin_port = htons(2020);
    // my_Addr->sin_zero=memset();
    int statusOfIP = inet_pton(AF_INET, "192.168.1.253", &(my_Addr->sin_addr.s_addr));
    if (statusOfIP <= 0)
    {
        if (statusOfIP == 0)
        {
            printf("Invalid string was a given as the IP address.");
        }
        else
        {
            fprintf(stderr, "invalid AddressFamily for the ip address.\nerror Type: %s",errno);
        }
    }


    int Binded = bind(Socketfd, (struct sockaddr *)&my_Addr, sizeof(my_Addr));
    if (Binded < 0)
    {

        dprintf(2, "Socket did not get binded at line number %i", (__LINE__ - 3));
    }

    int startedlistening = listen(Socketfd, SOMAXCONN);

    if (startedlistening < 0)
    {
        dprintf(2, "Socket did not started listening at line number %i", (__LINE__ - 4));
    }
    

    int connectedSocketfd = accept(Socketfd, (struct sockaddr *)&peer_Addr, &sizeof(peer_Addr)); // the peer ip address is now at peeraddr pointer address.
    // have to check if connected socket got initialized

    inet_ntop(sizeof(peer_Addr),peer_Addr->ss_family,peer_Addr->,peerIp);

    connect(connectedSocketfd, (struct sockaddr *)&peer_Addr, sizeof(peer_Addr));
}