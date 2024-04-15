#ifndef __BAKERY_H__
#define __BAKERY_H__

typedef enum
{
    GET_NUMBER = 1,
    GET_SERVICE,
} service_t;

typedef struct data
{
    unsigned char index;
    unsigned char number;
    unsigned char letter;
    char __alignment;
} data_t;

#endif // __BAKERY_H__
