#include <stdio.h>
#include <string.h>
#include "ethercat.h"

int main()
{
    void *pIOMap = NULL;
    if(ec_config_map(pIOMap))
    {
        printf("ec_config_map failed\n");
        return 1;
    }
    printf("Coucou\n");
    return 0;
}