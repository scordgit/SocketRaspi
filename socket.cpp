#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define PORT    50020


int main(int argc, char * argv[])
{
    int listenfd = 0, connfd = 0, NumBytes = 0;
    unsigned int AddrLen = 0;
    struct sockaddr_in serv_addr, other_addr;
    AddrLen = sizeof(other_addr);

    char sendBuff[256];
    char recieveBuff[256];
    time_t ticks;

    /*Set up socket, AF_INet for IPv4 and SOCK_DGRAM for UDP*/
    listenfd = socket(AF_INET, SOCK_DGRAM, 0);
    /*Check to make sure no error in socket*/
    if(listenfd < 0)
    {
        printf("Socket Error : %s\n", strerror(errno));
        return -2;
    }
    /*Set stuff to 0 before populating to ensure no weirdness*/
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET; //IPv4
    /*htonl converts values between host and network byte order (endianess)*/
    /*INADDR_ANY is used when we don't want to bind a socket to any specific IP
    This is useful we we don't know the IP address of OUR machine, we can use 
    INADDR_ANY*/
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    /*htons is host byte order to network byte order*/
    serv_addr.sin_port = htons(PORT);

    if ( bind(listenfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) == -1)
    {
        printf("Bind Error : %s\n", strerror(errno));
        return -3;
    }

    /*Marks the socket referred to by sockfd as a PASSIVE socket, that is, 
    as a socket that will be used to accept incoming conection requests using accept()
    Does NOT work unless the socket is set to SOCK_STREAM or SOCK_SEQPACKET which it is not*/
    /*The second argument defines the max length to which thte queue of connections may grow*/
    /*listen(listenfd,10);*/

    while(1)
    {
        strcpy(&sendBuff[0], "Discover Target");

        #ifdef USE_SEND
        if (sendto(listenfd, &sendBuff[0], sizeof(sendBuff), 0 , (struct sockaddr*) &other_addr, sizeof(other_addr)) == -1)
        {
            printf("Send Error : %s\n", strerror(errno));
            return -4;
        }
        #endif
        /*Look for Recieve Packets*/
        NumBytes = recvfrom(listenfd, &recieveBuff[0], sizeof(recieveBuff), 0, (struct sockaddr*) &other_addr, &AddrLen);
        if (NumBytes > 0)
        {
            printf("Bytes Recieved : %d\n", NumBytes);
            printf("Recieved packet from  IP %s : PORT %d\n", inet_ntoa(other_addr.sin_addr), ntohs(other_addr.sin_port));
            printf("Data: %s\n", recieveBuff);
        }
        else if (NumBytes == -1)
        {
            printf("Recvfrom Error : %s\n", strerror(errno));
        }
        
    }

    close(listenfd);
    return 0;

}