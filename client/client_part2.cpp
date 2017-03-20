//
//  client_part2.cpp
//  client
//
//  Created by Snehil Vishwakarma on 9/20/15.
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
#include <sys/time.h>

//MAIN FUNCTION

int main(int argc, char *argv[])
{
    int sfd,port;
    long int n;
    struct sockaddr_in sadd;
    char mem[256],conn_type[3];
    struct hostent *server;
    
    timeval time1,time2;
    double t1,t2;
    
    if (argc != 5)       //Check for having exactly 4 command line arguments(Server_IP Port_number Type_of_connection File_name)
    {
        std::cout<<"\nUser Error: Exactly 4 command line arguments needed!\n";
        exit(0);
    }
    
    port = atoi(argv[2]);
    
    sfd = socket(AF_INET, SOCK_STREAM, 0);      //Creating STREAM Socket for TCP connection
    if (sfd < 0)
    {
        std::cout<<"\nInternal Error: Socket cannot be opened!\n";
        exit(0);
    }
    
    server = gethostbyname(argv[1]);        //Getting ther server's name from CLI arguments
    
    if (server == NULL)
    {
        std::cout<<"\nInternal Error: no such host\n";
        exit(0);
    }
    
    bzero((char *) &sadd, sizeof(sadd));
    sadd.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&sadd.sin_addr.s_addr,
          server->h_length);
    
    sadd.sin_port = htons(port);
    
    if (connect(sfd,(struct sockaddr *) &sadd,sizeof(sadd)) < 0)     //Sending TCP connection request to server
    {
        std::cout<<"\nInternal Error: Error connecting to server!\n";
        exit(0);
    }
    
    bzero(conn_type,3);
    strcpy(conn_type,argv[3]);
    
    bzero(mem,256);
    strcpy(mem,argv[3]);
    strcat(mem," ");
    strcat(mem, argv[4]);
    
    n = write(sfd,mem,strlen(mem));     //Sending type_of_connection and file_name to the server
    if (n < 0)
    {
        std::cout<<"\nInternal Error: Writing to socket!\n";
        exit(0);
    }
    
    int count=0,pcount=0,i=0,j=0;
    
    bzero(mem,256);
    n=read(sfd,mem,256);        //Receiving number of files to be expected from the server
    count=atoi(mem);
    
    for(i=0;i<count;i++)        //If the file exists on the server
    {
        bzero(mem,256);
        
        gettimeofday(&time1, NULL);
        t1=time1.tv_sec+(time1.tv_usec/1000000.0);
        
        n=read(sfd,mem,256);
        pcount=atoi(mem);
        
        if(pcount==0)        //If the file does NOT exist on the server
        {
            bzero(mem,256);
            n = read(sfd,mem,255);
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Reading from socket!\n";
                exit(0);
            }
            std::cout<<"\n 404 Not Found\n";
        }
        else
            std::cout<<"\nFile "<<(i+1)<<": \n";
        for(j=0;j<pcount;j++)
        {
            bzero(mem,256);
            n = read(sfd,mem,255);
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Reading from socket!\n";
                exit(0);
            }
            
            gettimeofday(&time2, NULL);
            t2=time2.tv_sec+(time2.tv_usec/1000000.0);
        
            std::cout<<mem;
        }
        std::cout<<"\n Time taken: "<<t2-t1;
    }
    
    std::cout<<"\n";
    close(sfd);     //Closing the binded socket
    return 0;
}
