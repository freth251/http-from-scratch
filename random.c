#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define MAXLINE 1024
int main() {
    char content[MAXLINE];
    // Seed the random number generator
    srand(time(NULL));

    // Generate a random number
    int randomNumber = rand();

    sprintf(content, "<!DOCTYPE html>\n");
    sprintf(content, "%s<html>\n", content);
    sprintf(content, "%s<head>\n", content);
    sprintf(content, "%s<title>Random Number Generator</title>\n", content);
    sprintf(content, "%s</head>\n", content);
    sprintf(content, "%s<body body bgcolor=""000000"">\n", content);
    sprintf(content, "%s<h1 style=\"color: #00FF00;\">Generated Random Number</h1>\n", content);
    sprintf(content, "%s<p style=\"color: #00FF00;\">%d</p>\n", content, randomNumber);
    sprintf(content, "%s</body>\n", content);
    sprintf(content, "%s</html>\n", content);

    // Print the HTML output
    printf("Content-length: %d\r\n", (int)strlen(content));
    printf("Content-Type: text/html\n\n");

    printf("%s", content);

    fflush(stdout);
    

    return 0;
}
