#include <unistd.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <signal.h>

#define SOCK_NAME "server.soc"

int sockfd;

void sig_handler()
{
    unlink(SOCK_NAME);
    close(sockfd);
    exit(0);
}

int main()
{
    struct sockaddr serv_addr, clnt_addr;
    struct sigaction sa;
    int socklen, bytes;
    char buf[256], tmp[256];
    unsigned int addrlen;

    sockfd = socket(AF_UNIX, SOCK_DGRAM, 0);

    if (sockfd < 0)
    {
        perror("socket");
        exit(1);
    }

    serv_addr.sa_family = AF_UNIX;
    strcpy(serv_addr.sa_data, SOCK_NAME);

    socklen = strlen(serv_addr.sa_data)
        + sizeof(serv_addr.sa_family);
    if (bind(sockfd, &serv_addr, socklen) < 0)
    {
        perror("bind");
        exit(1);
    }

    sa.sa_handler = sig_handler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("sigaction");
        exit(1);
    }

    for (;;)
    {
        bytes = recvfrom(sockfd, buf, 256, 0, &clnt_addr, &addrlen);
        
        if (bytes < 0) 
        {
            perror("recvfrom");
            exit(1);
        }
        
        printf("recv %s from %s\n", buf, clnt_addr.sa_data);
        sprintf(tmp, "OK %s", buf);

        if (sendto(sockfd, tmp, strlen(tmp), 0, &clnt_addr, addrlen) < 0)
        {
            perror("sendto");
            exit(1);
        }
    }

    return 0;
}
