//
//  server3.cpp - FILE TRANSFER
//  Reliable UDP
//
//  Created by Snehil Vishwakarma on 10/8/15.
//  Copyright © 2015 Indiana University Bloomington. All rights reserved.
//

//HEADER FILES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fstream>

//MAIN FUNCTION

int main(int argc, char *argv[])
{
    int sfd,i=0,j=0,port;
    long int n;
    socklen_t len;
    struct sockaddr_in sadd,cadd;
    char mem[1024],fname[256];
    
    if (argc != 2)       //Check for having exactly 1 command line argument(Port Number)
    {
        std::cout<<"\nUser Error: Just one command line argument needed!\n";
        exit(0);
    }
    
    sfd=socket(AF_INET, SOCK_DGRAM, 0);      //Creating DATAGRAM Socket for UDP connection
    if (sfd < 0)
    {
        std::cout<<"\nInternal Error: Socket cannot be opened!\n";
        exit(0);
    }
    
    port = atoi(argv[1]);
    
    //Port Number Integrity Checks
    
    if(port<0)
    {
        std::cout<<"\nUser Error: Port number cannot be less than zero!\n";
        exit(0);
    }
    else if(port>=65536)
    {
        std::cout<<"\nUser Error: Port number cannot be more than 65535!\n";
        exit(0);
    }
    else if(port<=1023)
    {
        std::cout<<"\nUser Error: System ports cannot be used!\n";
        exit(0);
    }
    else if(port>=49152)
    {
        std::cout<<"\nUser Error: Reserved ports cannot be used!\n";
        exit(0);
    }
    
    bzero((char *) &sadd, sizeof(sadd));
    sadd.sin_family=AF_INET;
    sadd.sin_addr.s_addr=INADDR_ANY;
    sadd.sin_port=htons(port);
    if (bind(sfd,(struct sockaddr *)&sadd,sizeof(sadd))<0)     //Binding the socket to SERVER Address
    {
        std::cout<<"\nInternal Error: Cannot Bind!\n";
        exit(0);
    }
    
    len = sizeof(cadd);
    
    bzero(mem,1024);
    n = recvfrom(sfd,mem,1024,0,(struct sockaddr *)&cadd,&len); //Receiving File Name required
    if (n < 0)
    {
        std::cout<<"\nInternal Error: Reading from socket!\n";
        exit(0);
    }
    //std::cout<<"\n mem:"<<mem;
    
    bzero(fname,256);
    i=0;
    strcpy(fname, mem);     //Retreiving File Name required
    write(1, fname, n);
    
    std::ifstream fi;
    
    fi.open(fname,std::ios::binary);        //Opening File
    
    if(fi.is_open())        //If file exists
    {
        bzero(mem,1024);
        strcpy(mem,"1");
        n = sendto(sfd,mem,strlen(mem),
                   0,(struct sockaddr *)&cadd,len);
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
        
        //Calculating size of the file
        
        fi.seekg(0,std::ios::end);
        j=(int)fi.tellg();
        fi.seekg(0);
        std::cout<<"\n"<<j<<"\n";
        while (j>0)     //Sending file packets
        {
            bzero(mem,1024);
            if(j>=1024)
                fi.read(mem,1023);
            else
                fi.read(mem,j);
            n = sendto(sfd,mem,strlen(mem),
                       0,(struct sockaddr *)&cadd,len);
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            j=j-(int)(strlen(mem));
            std::cout<<"\n"<<j<<"\n";
        }
    }
    else        //If file does NOT exist
    {
        bzero(mem,1024);
        strcpy(mem,"0");
        n = sendto(sfd,mem,strlen(mem),
                   0,(struct sockaddr *)&cadd,len);
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
    }
    fi.close();     //Closing the opened file

    close(sfd);     //Closing the binded socket
    return 0;
}