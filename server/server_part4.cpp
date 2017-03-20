//
//  server_part2.cpp
//  server
//
//  Created by Snehil Vishwakarma on 9/20/15.
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
#include <sys/socket.h>
#include <fstream>
#include <pthread.h>
#include <netdb.h>
#include <arpa/inet.h>

//MAIN FUNCTION
void *send(void *ptr)
{
    int nsfd,i=0,j=0,k=0,l=0;
    long int n;
    char mem[256],conn_type[3],fname[256],arr[256];
    
    nsfd=(int)(long)ptr;        //Retreiving communicating socket connection
    bzero(mem,256);
    n = read(nsfd,mem,255);     //Receiving File Name
    
    if (n < 0)
    {
        std::cout<<"\nInternal Error: Reading from socket!\n";
        exit(0);
    }
    
    while(mem[k]!=' ')
        k++;
    bzero(conn_type,3);     //Retreiving connection type
    for(i=0;i<k;i++)
        conn_type[i]=mem[i];
    k++;
    bzero(fname,256);
    i=0;
    while(k<strlen(mem))        //Retreiving File Name required
        fname[i++]=mem[k++];
    
    std::ifstream fi,ftemp;
    
    if(conn_type[0]=='n' && conn_type[1]=='p')      //Non-persistent Connection (Single File Transmission)
    {
        fi.open(fname,std::ios::binary);        //Opening file as a binary file
        
        if(fi.is_open())        //If the file exists, send the file
        {
            
            bzero(mem,256);
            mem[0]='1';
            for(l=1;l<256;l++)
                strcat(mem," ");
            n=write(nsfd,mem,strlen(mem));      //Sending the index of file being sent (in this case 1, since there is only one file)
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            
            //Calculating size of the file
            
            fi.seekg(0,std::ios::end);
            j=(int)(fi.tellg()/255);
            if(fi.tellg()%255)
                j++;
            bzero(mem,256);
            
            strcpy(mem,"    ");
            i=j;
            k=0;
            l=j;
            while(i>0)
            {
                i=i/10;
                k++;
            }
            for(i=0;i<k;i++)
            {
                mem[k-i-1]=(l%10)+48;
                l=l/10;
            }
            
            k=256-strlen(mem);
            for(l=0;l<k;l++)
                strcat(mem," ");
            
            n=write(nsfd,mem,strlen(mem));      //Sending size of the file to the client
            
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            
            fi.seekg(0);
            for(i=0;i<j;i++)        //Transmitting the file
            {
                bzero(mem,256);
                fi.read(mem,255);
                n=write(nsfd,mem,strlen(mem));
                
                if (n < 0)
                {
                    std::cout<<"\nInternal Error: Writing to socket!\n";
                    exit(0);
                }
            }
            fi.close();
        }
        else        //If the file does NOT exist on server, send 404 NOT FOUND error
        {
            bzero(mem,256);
            mem[0]='0';
            for(l=1;l<256;l++)
                strcat(mem," ");
            n=write(nsfd,mem,strlen(mem));      //Sending 0 as no packet to send
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            bzero(mem,256);
            strcpy(mem,"404 Not Found");        //Sending 404 Error
            n=write(nsfd,mem,strlen(mem));
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            
        }
    }
    
    else if(conn_type[0]=='p')      //Persistent Connection (Multiple File Transmission)
    {
        fi.open(fname);     //Opening file as a text file
        
        if(fi.is_open())        //If the file exists, send the contents of the files in the listing file received
        {
            bzero(mem,256);
            j=0;
            while(!fi.eof())        //Calculating number of files to be sent
            {
                fi.getline(mem,256,'\n');
                j++;
            }
            
            //Writing count of files on the buffer
            
            bzero(mem,256);
            strcpy(mem,"    ");
            i=j;
            k=0;
            l=j;
            while(i>0)
            {
                i=i/10;
                k++;
            }
            for(i=0;i<k;i++)
            {
                mem[k-i-1]=(l%10)+48;
                l=l/10;
            }
            
            k=256-strlen(mem);
            for(l=0;l<k;l++)
                strcat(mem," ");
            
            n=write(nsfd,mem,strlen(mem));      //Sending the count of files to the client
            
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            
            fi.close();     //Closing the file
        }
        else        //If the file does NOT exist on server, send 404 NOT FOUND error
        {
            bzero(mem,256);
            strcpy(mem,"0  ");
            n=write(nsfd,mem,strlen(mem));      //Sending 0 as no packet to send
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            bzero(mem,256);
            strcpy(mem,"404 Not Found");        //Sending 404 Error
            n=write(nsfd,mem,strlen(mem));
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
        }
        
        fi.open(fname);     //Opening the list file as a text file
        if(fi.is_open())        //If the file is open
        {
            while(!fi.eof())
            {
                bzero(mem,256);
                fi.getline(mem,255,'\n');
                
                ftemp.open(mem,std::ios::binary);       //Opening each file one by one
                if(ftemp.is_open())
                {
                    //Calculating file size
                    
                    ftemp.seekg(0,std::ios::end);
                    j=(int)(ftemp.tellg()/255);
                    if(ftemp.tellg()%255)
                        j++;
                    
                    bzero(arr,256);
                    
                    strcpy(arr,"    ");
                    i=j;
                    k=0;
                    l=j;
                    while(i>0)
                    {
                        i=i/10;
                        k++;
                    }
                    for(i=0;i<k;i++)
                    {
                        arr[k-i-1]=(l%10)+48;
                        l=l/10;
                    }
                    
                    k=256-strlen(arr);
                    for(l=0;l<k;l++)
                        strcat(arr," ");
                    
                    n=write(nsfd,arr,strlen(arr));      //Writing number of iterations required at the client size
                    if (n < 0)
                    {
                        std::cout<<"\nInternal Error: Writing to socket!\n";
                        exit(0);
                    }
                    ftemp.seekg(0);
                    for(i=0;i<j;i++)        //Sending the file packets to client
                    {
                        bzero(arr,256);
                        ftemp.read(arr,255);
                        k=255-strlen(arr);
                        for(l=0;l<k;l++)
                            strcat(arr," ");
                        
                        n=write(nsfd,arr,strlen(arr));      //Sending packet to client
                        if (n < 0)
                        {
                            std::cout<<"\nInternal Error: Writing to socket!\n";
                            exit(0);
                        }
                    }
                    
                    ftemp.close();     //Closing the file
                }
                else
                {
                    bzero(mem,256);
                    strcpy(mem,"0  ");
                    n=write(nsfd,mem,strlen(mem));      //Sending 1 as no packet to send for this file
                    if (n < 0)
                    {
                        std::cout<<"\nInternal Error: Writing to socket!\n";
                        exit(0);
                    }
                    bzero(mem,256);
                    strcpy(mem,"404 Not Found");
                    n=write(nsfd,mem,strlen(mem));      //Sending 404 Error
                    if (n < 0)
                    {
                        std::cout<<"\nInternal Error: Writing to socket!\n";
                        exit(0);
                    }
                }
            }
            fi.close();     //Closing the file
        }
        else        //If the file does NOT exist on server, send 404 NOT FOUND error
        {
            bzero(mem,256);
            strcpy(mem,"0  ");
            n=write(nsfd,mem,strlen(mem));      //Sending 0 as no packet to send
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
            bzero(mem,256);
            strcpy(mem,"404 Not Found");        //Sending 404 Error
            n=write(nsfd,mem,strlen(mem));
            if (n < 0)
            {
                std::cout<<"\nInternal Error: Writing to socket!\n";
                exit(0);
            }
        }
    }
    else        //If NEITHER non-persistent NOR persistent connection
    {
        bzero(mem,256);
        strcpy(mem,"0  ");
        n=write(nsfd,mem,strlen(mem));      //Sending 0 as no packet to send
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
        bzero(mem,256);
        strcpy(mem,"400: Bad Request");
        n=write(nsfd,mem,strlen(mem));      //Sending 400: Bad Request
        if (n < 0)
        {
            std::cout<<"\nInternal Error: Writing to socket!\n";
            exit(0);
        }
    }
    
    std::cout<<"\n";
    
    close(nsfd);        //Closing the communicating socket
    
    pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
    int nsfd,sfd,port,v=0,n=0;
    socklen_t len;
    struct sockaddr_in sadd,cadd;
    pthread_t thread[100];
    int ns[100];
    fd_set  fdset;
    struct timeval timeout;
    
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
    
    FD_ZERO(&fdset);
    //FD_CLR(0, &fdset);
    FD_SET(sfd, &fdset);
    
    timeout.tv_sec=50;       //Timeout time if no reply from the server
    timeout.tv_usec=0;
    
    listen(sfd,5);      //Listening for clients at the binded socket
    
    while(select(sfd+1,&fdset, NULL, NULL, &timeout) && v<100)      //Listening for 50 seconds or 100 threads
    {
        len = sizeof(cadd);
        nsfd = accept(sfd,
                      (struct sockaddr *) &cadd,
                      &len);       //Accepting an incoming socket connection
        
        if (nsfd < 0)
        {
            std::cout<<"\nInternal Error: Cannot accept connection!\n";
            exit(0);
        }
        
        ns[v] = pthread_create( &thread[v], NULL, send, (void *)(long)nsfd);    //Creating NEW Thread
        
        if (ns[v])
        {
            std::cout<<"\nInternal Error: Unable to create thread! "<<ns[v]<<"\n";
            exit(0);
        }
        pthread_join( thread[v], NULL);     //Joining threads for concurrent processing
        v++;
        
    }
    
    pthread_exit(NULL);     //Exiting main thread
    close(sfd);     //Closing the binded socket
    return 0;
}

