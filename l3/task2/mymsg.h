#ifndef __COMMON_H__
#define __COMMON_H__

struct mymsg {
    int pid;
    char name[32];
    char data[1024 - sizeof(int) - sizeof(char[32])];
};

#endif // __COMMON_H__
