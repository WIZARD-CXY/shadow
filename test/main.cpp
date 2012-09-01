#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <stdlib.h>
#include "transithub.h"

using namespace std;

int main()
{
    int server_sockfd, client_sockfd;
    int server_len, client_len;
    struct sockaddr_un server_address;
    struct sockaddr_un client_address;

    transitHub dev;

    dev.portOpen(1);


/*  Remove any old socket and create an unnamed socket for the server.  */

    unlink("motion_socket");
    server_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

/*  Name the socket.  */

    server_address.sun_family = AF_UNIX;
    strcpy(server_address.sun_path, "motion_socket");
    server_len = sizeof(server_address);
    bind(server_sockfd, (struct sockaddr *)&server_address, server_len);

/*  Create a connection queue and wait for clients.  */

    listen(server_sockfd, 5);

    printf("Motion_Server is waiting\n");
    while(1) {
        int num;
/*  Accept a connection.  */

        client_len = sizeof(client_address);
        client_sockfd = accept(server_sockfd,(struct sockaddr *)&client_address, (socklen_t*)&client_len);

        printf("accept a client\n");
/*  We can now read/write to client on client_sockfd.  */

        read(client_sockfd, &num, 4);

        dev.doWriteCommandTx(22,num%1000,PACKET_SET_INIT);
        dev.doWriteCommandTx(21,num/1000,PACKET_SET_INIT);


        write(client_sockfd, &num, 4);
        close(client_sockfd);
    }


}
