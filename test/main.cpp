#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include "transithub.h"

using namespace std;

int main(int argc,char** argv)
{
    transitHub dev;

    dev.portOpen(1);

     int num=atoi(argv[1]);

     printf("Accept a client request \n");
     printf("21 : %d\n",num/1000);
     printf("22 : %d\n",num%1000);

     dev.doWriteCommandTx(22,num%1000,PACKET_SET_INIT);
     dev.doWriteCommandTx(21,num/1000,PACKET_SET_INIT);
}
