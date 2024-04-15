#ifndef __RESPONSE_H__
#define __RESPONSE_H__

#include "bakery.h"

typedef enum
{
    OK = 0,
    ERR_UNKNOWN_SERVICE_REQUEST,
    ERR_TIMEOUT,
} status_t;

typedef struct response
{
    status_t status;
    data_t data;
} response_t;

#endif // __RESPONSE_H__
