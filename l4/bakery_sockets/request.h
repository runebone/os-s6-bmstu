#ifndef __REQUEST_H__
#define __REQUEST_H__

#include <sys/types.h>
#include "bakery.h"

typedef struct request
{
    pid_t pid;
    service_t req;
    data_t data;
} request_t;

#endif // __REQUEST_H__
