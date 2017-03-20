//
//  main.cpp
//  server
//
//  Created by Snehil Vishwakarma on 9/18/15.
//  Copyright © 2015 Indiana University Bloomington. All rights reserved.
//

/* A simple server in the internet domain using TCP
 The port number is passed as an argument */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/socket.h>


void error(const char *msg)
{
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[])
{
    int sockfd;   //Value returned by socket system call
    int newsockfd;  //Value returned by socket ACCEPT system call
    
    int portno;   ////PORT NUMBER open at SERVER
    
    socklen_t clilen;          //Size of ADDRESS of CLIENT
    
    char buffer[256];            //READ Characters from socket connection
    
    struct sockaddr_in serv_addr, cli_addr;      /* sockaddr_in is STRUCTURE containing INTERNET ADDRESS
                                                    server_addr is SERVER INTERNET ADDRESS
                                                    cli_addr is CLIENT INTERNET ADDRESS which connects*/
    
    /* sockaddr_in HAS
        1. short sin_family    - must be AF_INET
        2. u_short sin_port
        3. struct in_addr sin_addr      - "in_addr" is a STRUCTURE, with one field, an unsigned long s_addr
        4. char sin_zero[8]             - NOT USED, must be ZERO
     */
    
    long int n;           //Return Value(length of message) for read() and write() calls
    
    if (argc < 2) {                //Check for No. of Arguments (argc)
        std::cout<<"ERROR, no port provided\n";
        exit(1);
    }
    
    sockfd = socket(AF_INET, SOCK_STREAM, 0);      /* CREATES a new SOCKET. Takes 3 arguments
                                                        1. Address Domain //must be AF_INET
                                                        2. Type of socket out of the two possible
                                                            a. SOCK_STREAM
                                                            b. SOCK_DGRAM
                                                        3. Protocol - Autoselected by OS
                                                            a. TCP for SOCK_STREAM
                                                            b. UDP for SOCK_DGRAM
                                                    */
    
    if (sockfd < 0)             //To check if SOCKET CALL Fails, returns -1 if it fails
        error("ERROR opening socket");
    
    bzero((char *) &serv_addr, sizeof(serv_addr));  /* Sets all values in buffer to 0
                                                        Takes 2 arguments
                                                        1. Pointer to buffer
                                                        2. Size of buffer
                                                        
                                                     This call here initializes "serv_addr" to 0's */
    
    portno = atoi(argv[1]);          /* Converts string command line argument (argv[1]) to required integer
                                         PORT NUMBER */
    
    //Assigning all data member values of structure "sockaddr_in" to it's object "server_addr"
    
    serv_addr.sin_family = AF_INET;   //1st data member value, sin_family
    
    serv_addr.sin_addr.s_addr = INADDR_ANY;    //Assigning the server IP Address fetched by "INADDR_ANY"
    
    serv_addr.sin_port = htons(portno);      /* Assigning PORT NUMBER
                                                htons() converts portno in HOST BYTE ORDER
                                                              to portno in NETWORK BYTE ORDER */
    
    /* bind() - binds a socket to an address, IN THIS CASE address of current host and port number of server
        It takes 3 arguments
        1. Socket File Descriptor                   - (sockfd)
        2. ADDRESS to which it is bound             - IN THIS CASE, (serv_addr) Server Address
        3. SIZE of ADDRESS to which it is bound     - IN THIS CASE, sizeof(serv_addr)
        
        Returns -1 if bind fails
     
     */
    
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    
    listen(sockfd,5);           /* listen() OPENS the PORT and starts LISTENING on server side
                                    2nd Argument is size of backlog queue (Number of connection requests by other clients, it can MAX be 5 for most systems) */
    
    clilen = sizeof(cli_addr);       //Assigning IPv4 size to variable clilen
    
    std::cout<<"\n Client address length: "<<clilen;
    std::cout<<"\n Client address: "<<cli_addr.sin_addr.s_addr;
    std::cout<<"\n Client port: "<<cli_addr.sin_port;
    
    /* ACCEPT blocks the PROCESS (Could even be a THREAD) until a CLIENT connects.
        It returns a NEW Socket File Descriptor (newsockfd). ALL the communication is done with this SFD
        The second argument is a reference pointer to CLIENT ADDRESS
        And the third argument is the size of this structure.
     */
    
    newsockfd = accept(sockfd,
                       (struct sockaddr *) &cli_addr,
                       &clilen);
    
    /* If accept returns -1, it means the CONNECTION to the CLIENT failed due to some reason */
    
    if (newsockfd < 0)
        error("ERROR on accept");
    
    bzero(buffer,256); //The buffer doing communication should be set to 0
    
    n = read(newsockfd,buffer,255); /* read() recieves message from the CLIENT. It BLOCKS the process till a
                                     write() is exectued on the other side.
                                     It returns number of characters recieved
                                     Takes 3 arguments
                                     1. Socket File Descriptor   - Remember to use the new one
                                     2. The buffer to store the messages being received
                                     3. MINIMUM (size of message recieved, 255)
                                     */
    
    if (n < 0) error("ERROR reading from socket");   //ERROR in reading if occurs
    
    std::cout<<"\n\n Here is the message: "<<buffer<<"\n";
    
    /* Both ends can both read and write to the connection.
     */
    
    n = write(newsockfd,"I got your message",18); /* write() - sends data to CLIENT with 3 arguments
                                                   1. Socket File Descriptor   - Remember to use the new one
                                                   2. The message to be sent
                                                   3. Size of the message
                                                   */
    
    if (n < 0) error("ERROR writing to socket"); //ERROR in writing if occurs
    
    close(newsockfd);      //Closing the COMMUNICATION channel socket
    
    close(sockfd);         //Closing the PORT channel socket
    
    return 0;
}