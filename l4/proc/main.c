#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
/* #include <limits.h> */
/* #include <fcntl.h> */
/* #include <linux/types.h> */
#include <memory.h>
#include <dirent.h>
#include <pthread.h>

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif // BUFSIZE

#ifndef ENV
#define ENV 0
#endif // ENV

#ifndef NTHREADS
#define NTHREADS 64
#endif // NTHREADS

/* Задание: написать программу, которая в пользовательском режиме выводит на экран: */
/* [x] Информацию об окружении процесса (environ) с комментариями; */
/* [x] Информацию о состоянии (state) процесса с комментариями; */
/* [x] Вывести информацию из файла cmdline и директории fd; */
/* [x] Вывести содержание символической ссылки cwd; */
/* [x] Вывести содержание символической ссылки exe; */
/* [x] Вывести содержание символической ссылки root; */
/* [x] Вывести содержание файла comm; */
/* [x] Вывести содержимое файла io; */
/* [x] Вывести содержимое файла maps; */
/* [x] Вывести содержимое поддиректории task. */
/* [x] Вывести root; */
/* [another file] Вывести pagemap; */

void *thread_func(void *args)
{
    void *m;
    for (int i = 0; i < 1000; i++) {
        m = malloc(4096 * 1024);
        memset(m, 0, sizeof(m));
        usleep(1000);
        free(m);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    pthread_t th[NTHREADS - 1];
    for (int i = 0; i < NTHREADS - 1; i++)
    {
        pthread_create(th + i, NULL, thread_func, NULL);
    }

    // /proc/self/stat
    {
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/stat", "r");
        fread(buf, 1, BUFSIZE, f);
        printf("%s", buf);
        fclose(f);
    }

    // /proc/self/cmdline
    {
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/cmdline", "r");
        fread(buf, 1, BUFSIZE, f);
        printf("cmdline: %s\n", buf);
        fclose(f);
    }

    // /proc/self/fd
    {
        printf("fd:\n"); 
        struct dirent *dirp; 
        DIR *dp; 
        char str[BUFSIZE + 1]; 
        memset(str, 0, sizeof(str));
        char path[BUFSIZE + 1]; 
        memset(path, 0, sizeof(path));
        dp = opendir("/proc/self/fd");
        while ((dirp = readdir(dp)) != NULL)
        { 
            if ((strcmp(dirp->d_name, ".") != 0) && 
                    (strcmp(dirp->d_name, "..") != 0)) 
            { 
                sprintf(path, "%s%s", "/proc/self/fd/", dirp->d_name); 
                memset(str, 0, sizeof(str));
                int nc = readlink(path, str, BUFSIZE); 
                printf("%s -> %s\n", dirp->d_name, str); 
            } 
        } 
        closedir(dp);
    }

    // /proc/self/cwd, exe, root
    {
        printf("cwd, exe, root:\n");
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        int nc = readlink("/proc/self/cwd", buf, BUFSIZE);
        printf("cwd -> %s\n", buf);
        nc = readlink("/proc/self/exe", buf, BUFSIZE);
        printf("exe -> %s\n", buf);
        nc = readlink("/proc/self/root", buf, BUFSIZE);
        printf("root -> %s\n", buf);
    }

    // /proc/self/comm, io
    {
        int len = 0;
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/comm", "r");
        fread(buf, 1, BUFSIZE, f);
        printf("\ncomm: %s\n", buf);
        fclose(f);
        printf("io:\n");
        f = fopen("/proc/self/io", "r");
        while ((len = fread(buf, 1, BUFSIZE, f)) > 0)
        {
            printf("%s\n", buf);
        }
        fclose(f);
    }

    // /proc/self/maps
    {
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/maps", "r");
        printf("maps:\n");
        while (fread(buf, 1, BUFSIZE, f) > 0)
        {
            printf("%s", buf);
        }
        printf("\n");
        fclose(f);
    }

    // /proc/self/task
    {
        struct dirent *dirp; 
        DIR *dp; 
        char str[BUFSIZE + 1]; 
        memset(str, 0, sizeof(str));
        char path[BUFSIZE + 1]; 
        memset(path, 0, sizeof(path));
        dp = opendir("/proc/self/task");
        int flag = 0;
        while ((dirp = readdir(dp)) != NULL)
        { 
            if ((strcmp(dirp->d_name, ".") != 0) && 
                    (strcmp(dirp->d_name, "..") != 0)) 
            { 
                if (!flag)
                {
                    sprintf(path, "%s%s", "/proc/self/task/", dirp->d_name);
                    flag = 1;
                }
                printf("\ntask/%s", dirp->d_name); 
            } 
        } 
        closedir(dp);
        printf("\n%s:\n", path); 
        dp = opendir(path);
        while ((dirp = readdir(dp)) != NULL)
        { 
            if ((strcmp(dirp->d_name, ".") != 0) && 
                    (strcmp(dirp->d_name, "..") != 0)) 
            { 
                printf("    %s\n", dirp->d_name); 
            } 
        } 
        closedir(dp);
    }

    // /proc/self/root
    {
        printf("\nroot:\n");
        struct dirent *dirp; 
        DIR *dp; 
        char str[BUFSIZE + 1]; 
        memset(str, 0, sizeof(str));
        char path[BUFSIZE + 1]; 
        memset(path, 0, sizeof(path));
        dp = opendir("/proc/self/root");
        while ((dirp = readdir(dp)) != NULL)
        { 
            if ((strcmp(dirp->d_name, ".") != 0) && 
                    (strcmp(dirp->d_name, "..") != 0)) 
            { 
                printf("    %s\n", dirp->d_name); 
            } 
        } 
        closedir(dp);
    }

    // /proc/self/environ
    if (ENV)
    {
        int len = 0;
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/environ", "r");
        while ((len = fread(buf, 1, BUFSIZE, f)) > 0)
        {
            for (int i = 0; i < len; i++)
            {
                if (buf[i] == 0)
                    buf[i] = '\n';
            }
            printf("%s", buf);
        }
        printf("\nCLOSING /proc/self/environ\n");
        fclose(f);
    }

    void *m = malloc(4096 * 1024);
    memset(m, 10, sizeof(m));
    printf("<Enter> to exit: ");
    read(0, NULL, 1);
    free(m);

    for (int i = 0; i < NTHREADS - 1; i++)
    {
        pthread_join(th[i], NULL);
    }

    /* pause(); */
}
