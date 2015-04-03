#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <net/if.h>

#include "utils.h"

int main(int argc, char *argv[])
{
    if(argc < 2){
        printf("Usage: %s eth0\n",argv[0]);
        return 1;
    }
    char* mac=readDeviceMac(argv[1]);
    if(mac!=NULL){
        printf("MAC:%s\n",mac);
    }else{
        printf("Can not find mac for device %s\n",argv[1]);
    }
                
    return 0;
}