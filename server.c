#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#define BACKLOG_LEN 1024
#define MAXLINE 1024
#define MAXBUF 10000


int create_server(int port){
    
    int sockfd= socket(AF_INET, SOCK_STREAM, 0 );

    if(sockfd==-1){
        printf("Error creating socket");
        return -1;
    }

    struct sockaddr_in serveraddr;

    serveraddr.sin_family= AF_INET;

    serveraddr.sin_addr.s_addr=htonl(INADDR_ANY);

    serveraddr.sin_port= htons((unsigned short)port);

    if (bind(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr))<0){
        printf("Error binding socket");
        return -1;
    }

    if (listen(sockfd, BACKLOG_LEN)<0){
        printf("Error listening socket");
        return -1;
    }

    return  sockfd;


}

void clienterror(int fd, char *cause, char *errnum, 
             char *shortmsg, char *longmsg)
{
    char buf[MAXLINE], body[MAXLINE];
    /* Build the HTTP response body */
    sprintf(body, "<html><title>Error</title>");
    sprintf(body, "%s<body bgcolor=""000000"">\r\n", body);
    sprintf(body, "%s<p style=\"color: #00FF00;\">%s: %s\r\n", body, errnum, shortmsg);
    sprintf(body, "%s<p style=\"color: #00FF00;\">%s: %s\r\n", body, longmsg, cause);
    sprintf(body, "%s<hr><em style=\"color: #00FF00;\">WEB SERVER</em>\r\n", body);
    /* Print the HTTP response */
    sprintf(buf, "HTTP/1.0 %s %s\r\n", errnum, shortmsg);
    write(fd, buf, strlen(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    write(fd, buf, strlen(buf));
    sprintf(buf, "Content-length: %d\r\n\r\n", (int)strlen(body));
    write(fd, buf, strlen(buf));
    write(fd, body, strlen(body));
}

int parse_uri(char *uri, char *filename, char *cgiargs){
    char *ptr;
    if (!strstr(uri, "/cgi-bin")){
        strcpy(filename, ".");
        if (strcmp(uri, "/")==0){
            strcat(filename, "/index.html");
        }else{
            strcat(filename, uri);
            
        }
        return 1;
        
    }else{
        ptr=index(uri, '?');
        if (ptr){
            strcpy(cgiargs, ptr+1);
            *ptr='\0';
        }
        else{
            strcpy(cgiargs, "");
        }
        strcpy(filename, ".");
        strcat(filename, uri);

        return 0;

    }
}

int readfile(char* filename, int wd){
    struct stat sbuf;

    int fd= open(filename, O_RDONLY, 0);
    if (fd==-1){
        printf("open failed");
        return -1;
    }
    if (fstat(fd, &sbuf)<0){
        printf("stat failed");
        return -1; 
    };


    char* srcp= (char *)malloc(sbuf.st_size * sizeof(char));

    if (read(fd, srcp, sbuf.st_size)<0){
        printf("read failed");
        return -1;
    }
    
    if (write(wd, srcp, sbuf.st_size)<0){
        printf("write failed");
        return -1;
    };

    close(fd);

    free(srcp);

    return 0;
}

void get_filetype(char *filename, char *filetype)
{
    if (strstr(filename, ".html"))
        strcpy(filetype, "text/html");
    else if (strstr(filename, ".gif"))
        strcpy(filetype, "image/gif");
    else if (strstr(filename, ".jpg"))

             strcpy(filetype, "image/jpeg");
         else
             strcpy(filetype, "text/plain");
}

void serve_static(int fd, char *filename, int filesize){
    char filetype[MAXLINE], buf[MAXBUF];
    /* Send response headers to client */
    get_filetype(filename, filetype);
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);
    sprintf(buf, "%sContent-length: %d\r\n", buf, filesize);
    sprintf(buf, "%sContent-type: %s\r\n\r\n", buf, filetype);
    write(fd, buf, strlen(buf));
    /* Send response body to client */

    readfile(filename, fd);


}

void serve_dynamic(int fd, char *filename, char *cgiargs){
    char buf[MAXLINE], *emptylist[] = { NULL };
    sprintf(buf, "HTTP/1.0 200 OK\r\n");
    sprintf(buf, "%sServer: Tiny Web Server\r\n", buf);

    write(fd, buf, strlen(buf));

    if (fork()==0){
        //setenv()
        dup2(fd, STDOUT_FILENO);
        execv(filename, emptylist);
    }

    wait(NULL);
    
}

int serve(int fd){
    while(1){
        struct stat sbuf;
        char buf[MAXLINE], method[MAXLINE], uri[MAXLINE], version[MAXLINE], filename[MAXLINE], cgiargs[MAXLINE];
        ssize_t b_read= recv(fd, buf, sizeof(buf), 0);

        if(b_read==-1){
            printf("\nError reading socket\n");
            return -1;
        }else if (b_read==0)
        {
            printf("\nError socket connection closed\n");
            return -1;
        }

        sscanf(buf, "%s %s %s", method, uri, version);

        if (strcasecmp(method, "GET")) {
            clienterror(fd, method, "501", "Not Implemented",
                        "Web server does not implement this method");
            return -1; 
        }
        
        
        int is_static= parse_uri(uri, filename, cgiargs);

        if (stat(filename, &sbuf)<0){
            clienterror(fd, method, "403", "Forbidden",
                        "Couldn't find file");
            return -1; 
        };

        if (is_static)
        {
            serve_static( fd, filename, sbuf.st_size);
        }else if (is_static==0)
        {
            serve_dynamic(fd, filename, cgiargs);
        }else{
            clienterror(fd, method, "403", "Forbidden",
                        "Couldn't find file");
            return -1; 
        }
    
                    

    }
    return 0;


    
}

int main(int argc, char **argv){
    struct sockaddr_in clientaddr; 
    int port;

    if (argc != 2) {
        fprintf(stderr, "usage: %s <port>\n", argv[0]);
        return -1;
    }
    port = atoi(argv[1]);


    int listenfd= create_server(port);

    if (listenfd<0){
        return -1;
    }
    socklen_t clientlen= sizeof(clientaddr);
    while(1){
        int connfd = accept(listenfd, (struct sockaddr *) &clientaddr, &clientlen);
        if (fork()==0){
            close(listenfd);
            int res= serve(connfd);
            if (res<0){
                close(connfd);
                break;
            }
        }
        close(connfd);
    
    }

}