#include <sys/socket.h>
#include <unistd.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/syscall.h>
#include <sys/time.h>
#include <time.h>
#include <assert.h>
#include <signal.h>
#include "bakery.h"
#include "request.h"
#include "response.h"

#define TIME_FMT "%02d:%02d:%02d:%06ld"
#define TIMEOUT  10

static int choosing[128] = {0};
static int numbers[128] = {0};
static int pids[128] = {0};
static int idx = 0;
static char ch = 'a';

pid_t gettid()
{
    return syscall(SYS_gettid);
}

int get_max_number()
{
	int max_number = 0;
	for (int i = 0; i < 128; i++)
		if (numbers[i] > max_number)
			max_number = numbers[i];
	return max_number;
}

response_t get_number(pid_t pid, data_t data)
{
    response_t resp = {0};
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    struct timeval tv;

    assert(idx < 128);

    if (pids[data.index] == pid && numbers[data.index] != 0)
    {
        gettimeofday(&tv, NULL);
        printf("[Th %08d] Pid %d already has a number (%d) "
                TIME_FMT "\n", gettid(), pid, data.number,
                now->tm_hour, now->tm_min, now->tm_sec,
                tv.tv_usec);
        goto exit;
    }

    data.index = idx;
    idx++;
    pids[data.index] = pid;

	choosing[data.index] = 1;
	data.number = get_max_number() + 1;
	numbers[data.index] = data.number;
	choosing[data.index] = 0;

    gettimeofday(&tv, NULL);
    printf("[Th %08d] Gave number %d to pid %d at "
            TIME_FMT "\n", gettid(), data.number, pid,
            now->tm_hour, now->tm_min, now->tm_sec,
            tv.tv_usec);

exit:
    resp.status = OK;
    resp.data = data;
    return resp;
}

response_t get_service(pid_t pid, data_t data)
{
    response_t resp = {0};
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    struct timeval tv;

	for (int i = 0; i < 128; i++)
	{
        int k = 0;
		while (choosing[i]) {}
		while (numbers[i] != 0 && (numbers[i] < numbers[data.index] || 
								   numbers[i] == numbers[data.index] && 
								   pids[i] < pids[data.index]))
        {
            if (k == 0)
            {
                /* printf("[DEBUG] i = %d\nnumbers[i] = %d\ndata.index = %d\nnumbers[data.index] = %d\npids[i] = %d\npids[data.index] = %d\n\n", i, numbers[i], data.index, numbers[data.index], pids[i], pids[data.index]); */
                k = 1;
            }
        }
	}

    if (numbers[data.index] != 0)
    {
        data.letter = ch;
        ch++;

        gettimeofday(&tv, NULL);
        printf("[Th %08d] Gave letter '%c' to pid %d at "
                TIME_FMT "\n", gettid(), data.letter, pid,
                now->tm_hour, now->tm_min, now->tm_sec,
                tv.tv_usec);

        numbers[data.index] = 0;
    }
    else
    {
        gettimeofday(&tv, NULL);
        printf("[Th %08d] Number of pid %d expired by "
                TIME_FMT "\n", gettid(), pid,
                now->tm_hour, now->tm_min, now->tm_sec,
                tv.tv_usec);
    }

    data.number = 0;
    resp.status = OK;
    resp.data = data;
    return resp;
}

void *handle_connection(void *client_socket);

int main(int argc, char *argv[])
{
    printf("Server pid: %d\n", getpid());
    int serv_sock, clnt_sock, *pth_clnt_sock;
    struct sockaddr_in serv_addr, clnt_addr;
    socklen_t clnt_size = sizeof(clnt_addr);
    pthread_t thread_id;
    struct timeval timeout;
    timeout.tv_sec = TIMEOUT;
    timeout.tv_usec = 0;

    serv_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (serv_sock < 0)
    {
        perror("socket");
        exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(42069);

    if (bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("bind");
        exit(1);
    }

    if (listen(serv_sock, SOMAXCONN) < 0)
    {
        perror("listen");
        exit(1);
    }

    while ((clnt_sock = accept(serv_sock, (struct sockaddr *)&clnt_addr, &clnt_size)))
    {
        printf("New connection: %d\n", clnt_sock);

        pth_clnt_sock = malloc(sizeof(int));
        *pth_clnt_sock = clnt_sock;

        if (setsockopt(*pth_clnt_sock, SOL_SOCKET, SO_RCVTIMEO,
                (char*)&timeout, sizeof(timeout)) < 0)
        {
            perror("setsockopt");
            close(clnt_sock);
            exit(1);
        }

        handle_connection((void*)pth_clnt_sock);
    }

    if (clnt_sock < 0)
    {
        perror("accept");
        exit(1);
    }

    return 0;
}

void *handle_connection(void *client_socket)
{
    int clnt_sock = *(int*)client_socket;
    request_t req = {0};
    response_t resp = {0};
    ssize_t count;
    pid_t tid = gettid();
    char info[256] = {0};
    time_t mytime = time(NULL);
    struct tm *now = localtime(&mytime);
    struct timeval tv;

    if (1)
    {
        gettimeofday(&tv, NULL);
        printf("[Th %08d] is waiting for command from pid %d at "
                TIME_FMT "\n", tid, req.pid,
                now->tm_hour, now->tm_min, now->tm_sec,
                tv.tv_usec);

        count = recv(clnt_sock, &req, sizeof(request_t), 0);

        if (count > 0)
        {
            switch (req.req)
            {
                case GET_NUMBER:
                    snprintf(info, 255, "NUMBER");
                    break;
                case GET_SERVICE:
                    snprintf(info, 255, "SERVICE");
                    break;
                default:
                    snprintf(info, 255, "UNKNOWN");
                    break;
            }
            gettimeofday(&tv, NULL);
            printf("[Th %08d] Got request of %s from pid %d at "
                    TIME_FMT "\n", tid, info, req.pid,
                    now->tm_hour, now->tm_min, now->tm_sec,
                    tv.tv_usec);
            memset(info, 0, sizeof(info));
        }
        else
        {
            if (count < 0)
            {
                gettimeofday(&tv, NULL);
                fprintf(stderr, "[Th %08d] perror ", tid);
                perror("recv (probably TIMEOUT)");
                fprintf(stderr, " at " TIME_FMT "\n",
                        now->tm_hour, now->tm_min, now->tm_sec,
                        tv.tv_usec);
            }
            else
            {
                gettimeofday(&tv, NULL);
                fprintf(stdout, "[Th %08d] pid %d disconnected at "
                        TIME_FMT "\n", tid, req.pid,
                        now->tm_hour, now->tm_min, now->tm_sec,
                        tv.tv_usec);
            }

            if (pids[req.data.index] == req.pid)
            {
                numbers[req.data.index] = 0;
            }
            /* break; */
        }

        switch (req.req)
        {
            case GET_NUMBER:
                resp = get_number(req.pid, req.data);
                req.data = resp.data; // HACK
                break;
            case GET_SERVICE:
                resp = get_service(req.pid, req.data);
                req.data = resp.data; // HACK
                break;
            default:
                resp.status = ERR_UNKNOWN_SERVICE_REQUEST;
                resp.data = req.data;
                break;
        }

        if (send(clnt_sock, &resp, sizeof(resp), 0) < 0)
        {
            switch (resp.status)
            {
                case OK:
                    snprintf(info, 255, "OK");
                    break;
                case ERR_UNKNOWN_SERVICE_REQUEST:
                    snprintf(info, 255, "ERR_UNKNOWN_SERVICE_REQUEST");
                    break;
                case ERR_TIMEOUT:
                    snprintf(info, 255, "ERR_TIMEOUT (impossible)");
                    break;
            }
            gettimeofday(&tv, NULL);
            fprintf(stderr, "[Th %08d] perror (status=%s) ", tid, info);
            perror("send");
            fprintf(stderr, " at " TIME_FMT "\n",
                    now->tm_hour, now->tm_min, now->tm_sec,
                    tv.tv_usec);
            memset(info, 0, sizeof(info));
        }
    }

    if (count < 0)
    {
        resp.status = ERR_TIMEOUT;
        resp.data = req.data;
        if (send(clnt_sock, &resp, sizeof(resp), 0) < 0)
        {
            gettimeofday(&tv, NULL);
            fprintf(stderr, "[Th %08d] perror (status=TIMEOUT) ", tid);
            perror("send");
            fprintf(stderr, " at " TIME_FMT "\n",
                    now->tm_hour, now->tm_min, now->tm_sec,
                    tv.tv_usec);
        }
    }

    close(clnt_sock);
    free(client_socket);

    gettimeofday(&tv, NULL);
    fprintf(stdout, "[Th %08d] exited with code 0 at "
            TIME_FMT "\n", tid,
            now->tm_hour, now->tm_min, now->tm_sec,
            tv.tv_usec);

    return 0;
}
