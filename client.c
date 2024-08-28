#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define MAX_LINE 1024


int create_client(char * hostname, int port){
    struct hostent *hp; 
    struct sockaddr_in serveraddr;
    
    int sockfd= socket(AF_INET, SOCK_STREAM, 0 );

    if(sockfd<0){
        printf("Error creating socket");
        return -1;
    }
    if ((hp= gethostbyname(hostname))==NULL){
        printf("Error getting host");
        return -1; 
    }
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)hp->h_addr_list[0],
            (char *)&serveraddr.sin_addr.s_addr, hp->h_length);
    serveraddr.sin_port = htons(port);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))<0){
        printf("Error connecting to server");
        return -1; 
    }

    return  sockfd;


}

int receive(int fd){
    char buf[MAX_LINE];
    ssize_t b_read= recv(fd, buf, sizeof(buf), 0);

    if(b_read==-1){
        printf("Error reading socket");
        return -1;
    }else if (b_read==0)
    {
        printf("Error socket connection closed");
        return -1;
    }

    printf("\nClient recieved: %s\n\n", buf);
    return 0;


    
}

int main(int argc, char **argv){
    int port;
    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return -1;
    }
    port = atoi(argv[1]);
    int sockfd = create_client("localhost", port);

    if (sockfd<0){
        return -1;
    }
     

    while (1)
    {
        char buf[MAX_LINE];
        int b_read;
        b_read= read(STDIN_FILENO, buf, sizeof(buf));
        if (b_read<0){
            printf("Error reading from stdin");
            return -1;
        }else if (b_read == 0) {
            printf("Connection closed.\n");
            break;
        }
        


        send(sockfd, buf, sizeof(buf),0);

        int f= receive(sockfd);

        if (f<0){
            
            return-1;
        }

    }
    


}