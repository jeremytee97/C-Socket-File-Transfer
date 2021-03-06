/* A simple client program for server.c

   To compile: gcc client.c -o client

   To run: start the server, then the client */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#define LENGTH 256
ssize_t total = 0;
void sendfile(FILE *fp, int sockfd);
int main(int argc, char ** argv)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent * server;
    const char close_conn[] = "GOODBYE-CLOSE-TCP";


    if (argc < 3)
    {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    portno = atoi(argv[2]);

    /* Translate host name into peer's IP address ;
     * This is name translation service by the operating system
     */
    server = gethostbyname(argv[1]);

    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    /* Building data structures for socket */

    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;

    bcopy(server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);

    serv_addr.sin_port = htons(portno);

    /* Create TCP socket -- active open
    * Preliminary steps: Setup: creation of active open socket
    */

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0)
    {
        perror("ERROR opening socket");
        exit(0);
    }

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(0);
    }

   FILE* fp = fopen(argv[3], "rb");

   if (fp == NULL){
        perror("Cannot open file");
        exit(1);
   }

   sendfile(fp, sockfd);
   printf("Send Success, NumBytes = %ld\n", total);
   fclose(fp);
   close(sockfd);
    return 0;
}


void sendfile(FILE *fp, int sockfd)
{
    int n;
    char buffer[LENGTH];
    while ((n = fread(buffer, sizeof(char), LENGTH, fp)) > 0)
    {
	    total+=n;
        if (n != LENGTH && ferror(fp))
        {
            perror("Read File Error");
            exit(1);
        }

        if (send(sockfd, buffer, n, 0) < 0)
        {
            perror("Can't send file");
            exit(1);
        }
        memset(buffer, 0, LENGTH);
    }
}
