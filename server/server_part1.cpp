//
//  server_part1.cpp
//  server
//
//  Created by Snehil Vishwakarma on 9/20/15.
//  Copyright © 2015 Indiana University Bloomington. All rights reserved.
//

// HEADER FILES

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <fstream>
#include<sys/time.h>

//MAIN FUNCTION

int main(int argc, char *argv[])
{
    int sfd,newsfd,port,i,k,l;
    long int n;
    char mem[1024],arr1[1024],arr2[1024];
    socklen_t len;
    struct sockaddr_in sadd,cadd;
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //Creating STREAM Socket for TCP connection
    if (sfd < 0)
    {
        std::cout<<"\nInternal Error: Socket cannot be opened!\n";
        exit(0);
    }
    if (argc != 2)       //Check for having exactly 1 command line argument(Port Number)
    {
        std::cout<<"\nUser Error: Just one command line argument needed!\n";
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
    sadd.sin_family = AF_INET;
    sadd.sin_addr.s_addr = INADDR_ANY;
    sadd.sin_port = htons(port);
    
    if (bind(sfd, (struct sockaddr *) &sadd, sizeof(sadd)) < 0)     //Binding the socket to SERVER Address
    {
        std::cout<<"\nInternal Error: Cannot Bind!\n";
        exit(0);
    }
    
    listen(sfd,5);      //Listening for clients at the binded socket
    len = sizeof(cadd);
    newsfd = accept(sfd,
                       (struct sockaddr *) &cadd,
                       &len);       //Accepting an incoming socket connection
    
    if (newsfd < 0)
    {
        std::cout<<"\nInternal Error: Cannot accept connection!\n";
        exit(0);
    }
    
    bzero(mem,1024);
    n = read(newsfd,mem,1023);      //Reading client request in buffer
    if (n < 0)
    {
        std::cout<<"\nInternal Error: Reading from socket!\n";
        exit(0);
    }
    
    bzero(arr1,1024);
    k=0;
    l=k;
    while(mem[l]!=' ')
        l++;
    for(i=k;i<l;i++)
    {
        arr1[i-k]=mem[i];
    }
    
    if(strcmp(arr1,"GET")!=0)       //Check for bad request by client
    {
        bzero(mem,1024);
        strcpy(mem,"400: Bad Request");
        n=write(newsfd,"400: Bad Request",strlen(mem));     //Writing 400: Bad Request to client
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
        exit(0);
    }
        
    bzero(arr2,1024);
    k=l;
    while(mem[k]!='/')
        k++;
    k++;
    l=k;
    while(mem[l]!='.')
        l++;
    while(mem[l]!=' ')
        l++;
    for(i=k;i<l;i++)
        arr2[i-k]=mem[i];
    
    k=l+1;
    l=k;
    while(mem[l]!='/')
        l++;
    bzero(arr1,1024);
    for(i=k;i<l;i++)
    {
        arr1[i-k]=mem[i];
    }
    
    if(strcmp(arr1,"HTTP")!=0)      //Check for bad request by client
    {
        bzero(mem,1024);
        strcpy(mem,"400: Bad Request");
        n=write(newsfd,"400: Bad Request",strlen(mem));
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
        exit(0);
    }
    
    k=l+1;
    l=k;
    
    if(mem[l]!='1' || mem[l+1]!='.')        //Check for bad request by client
    {
        bzero(mem,1024);
        strcpy(mem,"400: Bad Request");
        n=write(newsfd,"400: Bad Request",strlen(mem));
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
        exit(0);
    }
    
    std::ifstream fi;
    
    fi.open(arr2);      //Opening the requested file
    
    if(fi.is_open())    //Only allow sending if the file exists on the server
    {
        bzero(mem,1024);
        l=1;
        while(l!=0)
        {
            k=0;
            while(!fi.eof() && k<1023)
            {
                fi.get(mem[k]);     //Reading character by character from file
                k++;
            }
            l=k;
            n=write(newsfd,mem,strlen(mem));    //Writing file on the socket(sending to the client)
            if(n<0)
                std::cout<<"\nInternal Error: Writing to socket!\n";
            bzero(mem,1024);
        }
    }
    else        //Sending Not Found File error to client
    {
        bzero(mem,1024);
        strcpy(mem,"404 Not Found");
        n=write(newsfd,"404 Not Found",strlen(mem));    //Writing 404: Not Found to client
        if (n < 0)
            std::cout<<"\n Internal Error: Writing to socket!\n";
    }
    fi.close();     //Closing the file
    close(newsfd);      //Closing the communicating socket
    close(sfd);     //Closing the binded socket
    return 0;
}

