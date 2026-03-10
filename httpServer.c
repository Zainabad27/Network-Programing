#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>

int main()
{
    int Socketfd = socket(AF_INET, SOCK_STREAM, 0);
    char buffer[128];

    if (Socketfd < 0)
    {
        dprintf(2, "Socket did not get initialized at line number %i", (__LINE__ - 5));
    }
    else
    {
        printf("Socket Got initialized with File descriptor %i", Socketfd);
    }

    struct sockaddr_in *my_Addr;
    my_Addr->sin_family = AF_INET;
    my_Addr->sin_port = htons(2020);
    my_Addr->sin_addr.s_addr = htonl("192.168.1.253");

    struct sockaddr_in *peer_Addr;

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

    int connectedSocketfd = accept(Socketfd, (struct sockaddr*)&peer_Addr, sizeof(peer_Addr));
    // have to check if connected socket got initialized



    connect(connectedSocketfd,(struct sockaddr*)&peer_Addr,sizeof(peer_Addr));

 
}