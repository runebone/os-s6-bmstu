#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <dirent.h>
#include <pthread.h>

#ifndef BUFSIZE
#define BUFSIZE 1024
#endif // BUFSIZE

int main(int argc, char *argv[])
{
    // /proc/self/stat
    if (1)
    {
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        char stat[BUFSIZE + 1];
        memset(stat, 0, sizeof(stat));
        if (argc > 1)
        {
            sprintf(stat, "/proc/%s/stat", argv[1]);
        }
        else
        {
            sprintf(stat, "/proc/self/stat");
        }
        FILE *f = fopen(stat, "r");
        fread(buf, 1, BUFSIZE, f);
        printf("%s", buf);
        fclose(f);
    }

    // /proc/self/cmdline
    if (0)
    {
        char buf[BUFSIZE + 1];
        memset(buf, 0, sizeof(buf));
        FILE *f = fopen("/proc/self/cmdline", "r");
        fread(buf, 1, BUFSIZE, f);
        printf("cmdline: %s\n", buf);
        fclose(f);
    }

    // /proc/self/fd
    if (0)
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
    if (0)
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
    if (0)
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
        char maps[BUFSIZE + 1];
        memset(maps, 0, sizeof(maps));
        if (argc > 1)
        {
            sprintf(maps, "/proc/%s/maps", argv[1]);
        }
        else
        {
            sprintf(maps, "/proc/self/maps");
        }
        FILE *f = fopen(maps, "r");
        printf("maps:\n");
        unsigned int start, end;
        /* while (fread(buf, 1, BUFSIZE, f) > 0) */
        while (fgets(buf, BUFSIZE, f) > 0)
        {
            *strrchr(buf, '\n') = 0;
            printf("%s", buf);
            sscanf(buf, "%x-%x", &start, &end);
            printf(" %d pages\n", (end - start) / getpagesize());
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
        int flag = 0;
        char task[BUFSIZE + 1];
        memset(task, 0, sizeof(task));
        if (argc > 1)
        {
            sprintf(task, "/proc/%s/task/", argv[1]);
        }
        else
        {
            sprintf(task, "/proc/self/task/");
        }
        dp = opendir(task);
        FILE *f = fopen(task, "r");
        while ((dirp = readdir(dp)) != NULL)
        { 
            if ((strcmp(dirp->d_name, ".") != 0) && 
                    (strcmp(dirp->d_name, "..") != 0)) 
            { 
                if (!flag)
                {
                    sprintf(path, "%s%s", task, dirp->d_name);
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
    if (0)
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
    if (0)
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
}
