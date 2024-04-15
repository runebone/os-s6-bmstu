#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include <signal.h>
#include "bakery.h"
#include "request.h"
#include "response.h"

int sock;

void sighandler()
{
    printf("\nbye-bye\n");
    close(sock);
    exit(0);
}

void sigpipehandler()
{
    printf("\nConnection timed out. Reconnect or exit.\n");
}

int main(int argc, char *argv[])
{
    printf("Client pid: %d\n", getpid());
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    if (argc > 1)
    {
        serv_addr.sin_addr.s_addr = inet_addr(argv[1]);
    }
    else
    {
        serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    }
    serv_addr.sin_port = htons(42069);

    struct sigaction sa;
    sa.sa_handler = sighandler;
    sa.sa_flags = 0;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGINT, &sa, NULL) < 0)
    {
        perror("sigaction");
        exit(1);
    }
    sa.sa_handler = sigpipehandler;
    if (sigaction(SIGPIPE, &sa, NULL) < 0)
    {
        perror("sigpipeaction");
        exit(1);
    }

    request_t req = {0};
    response_t resp = {0};
    req.pid = getpid();

    while (1)
    {
        sock = socket(AF_INET, SOCK_STREAM, 0);
        if (sock < 0)
        {
            perror("socket");
            exit(1);
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        {
            perror("connect");
            exit(1);
        }

        for (;;)
        {
            int x = 0;
            printf(
                "[1] Get number\n"
                "[2] Get letter\n"
                "[3] Reconnect\n"
                "[0] Exit\n"
                "> "
                );
            scanf("%d", &x);

            if (x == 1)
            {
                req.req = GET_NUMBER;
                if (send(sock, &req, sizeof(req), 0) < 0)
                {
                    perror("send 1");
                }
            }
            else if (x == 2)
            {
                req.req = GET_SERVICE;
                if (send(sock, &req, sizeof(req), 0) < 0)
                {
                    perror("send 2");
                }
            }
            else if (x == 3)
            {
                close(sock);
                printf("\nReconnecting...\n\n");
                break;
            }
            else
            {
                goto exit;
            }

            if (recv(sock, &resp, sizeof(resp), 0) < 0)
            {
                perror("recv");
            }
            else // XXX no else
            {
                printf("\nResponse status: %d", resp.status);
                if (resp.status == OK)
                {
                    req.data = resp.data;
                    printf("\nReceived data:\n"
                            "- index = %d\n"
                            "- number = %d\n"
                            "- letter = %c\n"
                            "\n",
                            req.data.index,
                            req.data.number,
                            req.data.letter);
                }
                else if (resp.status == ERR_TIMEOUT)
                {
                    printf(" (Timed out)\n\n");
                }
            }
        }
    }

exit:
    close(sock);

    return 0;
}
