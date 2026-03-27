#define _POSIX_C_SOURCE 200112L
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>

int main() {
    struct addrinfo hints, *res;
    int sockfd, new_fd;
    int status;

    // BLOCK 1: The "Set Up"
    memset(&hints, 0, sizeof hints);
    hints.ai_family   = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_PASSIVE;

    status = getaddrinfo(NULL, "8080", &hints, &res);
    if (status != 0) {
        // getaddrinfo has its own error system, so we use gai_strerror instead of perror
        fprintf(stderr, "getaddrinfo failed: %s\n", gai_strerror(status));
        return 1;
    }

    // BLOCK 2: The "Opening the Hole"
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == -1) {
        perror("socket");
        freeaddrinfo(res);
        return 1;
    }

    // Fix the "Address already in use" problem on quick restarts
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof yes) == -1) {
        perror("setsockopt");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1) {
        perror("bind");
        close(sockfd);
        freeaddrinfo(res);
        return 1;
    }

    // res is no longer needed after bind, free it early
    freeaddrinfo(res);

    if (listen(sockfd, 10) == -1) {
        perror("listen");
        close(sockfd);
        return 1;
    }

    printf("Server is up! Waiting for a buddy to connect...\n");

    // BLOCK 3: The "Handshake"
    new_fd = accept(sockfd, NULL, NULL);
    if (new_fd == -1) {
        perror("accept");
        close(sockfd);
        return 1;
    }

    // BLOCK 4: The "Talk"
    // send() can do partial sends, so we loop until everything is sent
    char *msg     = "Yo! You connected to my server, no cap.";
    size_t total  = strlen(msg);
    size_t sent   = 0;

    while (sent < total) {
        ssize_t n = send(new_fd, msg + sent, total - sent, 0);
        if (n == -1) {
            perror("send");
            close(new_fd);
            close(sockfd);
            return 1;
        }
        sent += n;
    }

    printf("Message sent successfully!\n");

    // CLEANUP
    close(new_fd);
    close(sockfd);
    return 0;
}