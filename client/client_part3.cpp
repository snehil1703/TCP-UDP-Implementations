//
//  client3.cpp - FILE TRANSFER
//  Reliable UDP
//
//  Created by Snehil Vishwakarma on 10/8/15.
//  Copyright © 2015 Indiana University Bloomington. All rights reserved.
//

//HEADER FILES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <fstream>

//MAIN FUNCTION

int main(int argc, char *argv[])
{
    int sfd;
    long int n;
    socklen_t len;
    struct sockaddr_in sadd,cadd;
    struct hostent *hp;
    char mem[1024];
    fd_set  fdset;
    struct timeval timeout;
    bool chk=true;
    
    if (argc != 4)       //Check for having exactly 3 command line arguments(Server_IP Port_number File_name)
    {
        std::cout<<"\nUser Error: Exactly 3 command line arguments needed!\n";
        exit(0);
    }
    
    sfd= socket(AF_INET, SOCK_DGRAM, 0);        //Creating DATAGRAM Socket for UDP connection
    if (sfd < 0)
    {
        std::cout<<"\nInternal Error: Socket cannot be opened!\n";
        exit(0);
    }
    
    hp = gethostbyname(argv[1]);        //Getting ther server's name from CLI arguments
    if (hp == NULL)
    {
        std::cout<<"\nInternal Error: no such host\n";
        exit(0);
    }
    
    bzero((char *) &sadd, sizeof(sadd));
    sadd.sin_family = AF_INET;
    bcopy((char *)hp->h_addr,
          (char *)&sadd.sin_addr,
          hp->h_length);
    sadd.sin_port = htons(atoi(argv[2]));
    
    len=sizeof(cadd);
    
    
    bzero(mem,1024);
    strcpy(mem,argv[3]);
    n=sendto(sfd,mem,
             strlen(mem),0,(const struct sockaddr *)&sadd,len);     //Sending required file's file_name to the server
    if (n < 0)
    {
        std::cout<<"\nInternal Error: Writing to socket!\n";
        exit(0);
    }
    
    FD_ZERO(&fdset);
    //FD_CLR(0, &fdset);
    FD_SET(sfd, &fdset);
    
    timeout.tv_sec=5;       //Timeout time if no reply from the server
    timeout.tv_usec=0;
    
    std::ofstream fi;
    
    //fi.open(argv[3],std::ios::binary);      //Opening the file to create
    
    while(select(sfd+1,&fdset, NULL, NULL, &timeout))
    {
        if(chk)
        {
            bzero(mem,1024);
            n = recvfrom(sfd,mem,1024,0,(struct sockaddr *)&cadd, &len);
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Reading from socket!\n";
                exit(0);
            }
            chk=false;
            if(mem[0]=='1')
                fi.open(argv[3],std::ios::binary);      //Opening the file if file exists on server
            else
            {
                std::cout<<"\n 404 Not Found\n";
                break;      //Break if the file does not exist
            }
        }
        bzero(mem,1024);
        n = recvfrom(sfd,mem,1024,0,(struct sockaddr *)&cadd, &len);        //Receiving file packets
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Reading from socket!\n";
            exit(0);
        }
        fi.write(mem, strlen(mem));     //Writing on the file on client side
    }
    
    fi.close();     //Closing the open file
    
    close(sfd);     //Closing the communicating socket
    return 0;
}