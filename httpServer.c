#include<sys/socket.h>
#include<netinet/in.h>
#include<stdio.h>




int main(){
    int Socketfd=socket(AF_INET,SOCK_STREAM,0);
    char  buffer[128];

    if(Socketfd<0){
        printf("Hello");

    }
    else{
           printf("hello from %s\nthe socket fd is: %i\n","zain",Socketfd);
    }


     struct  sockaddr * socketAddress;

    socketAddress->sa_family=AF_INET;
    // socketAddress->sa_data=buffer;

    bind(Socketfd,socketAddress,sizeof(socketAddress));
    


}