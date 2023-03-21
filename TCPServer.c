// Program-->TCP Server for file encryption and decryption..!!

#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}
const int shift = 100;

// Recieve file
void recieve_file(int newsockfd)
{
    FILE *fp;
    char buffer[255];
    int words, curr = 0;
    fp = fopen("file_recieved.txt", "w");

    // reads the recieved file
    read(newsockfd, &words, sizeof(int));
    printf("words: %d\n", words);

    int enc;
    read(newsockfd, &enc, sizeof(int));

    while (curr != words)
    {
        read(newsockfd, buffer, 255);
        for (int i = 0; i < strlen(buffer); i++)
        {
            buffer[i] += enc == 1 ? shift : -shift;
        }
        fprintf(fp, "%s ", buffer);
        curr++;
    }
    printf("The file has been recieved successfully [File Name: "
           "file_recieved.txt] \n");
    fclose(fp);
}

// fun send file
void send_file(int newsockfd)
{
    FILE *fs;
    char buffer[255];
    fs = fopen("file_recieved.txt", "r");
    int words = 0;
    char c;
    while ((c = getc(fs)) != EOF)
    {
        fscanf(fs, "%s", buffer);
        if (c == ' ' || c == '\t')
            words++;
    }
    write(newsockfd, &words, sizeof(int));
    rewind(fs);

    char ch;
    while (ch != EOF)
    {
        fscanf(fs, "%s", buffer);
        write(newsockfd, buffer, 255);
        ch = fgetc(fs);
    }
    printf("File successfully sent !!\n");
    fclose(fs);
}

// Driver code
int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        fprintf(stderr, "Port Number not provided. Program terminated.\n");
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t cli_len;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening Socket.");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    // binding socket
    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Binding Failed.");
    }
    listen(sockfd, 5);
    cli_len = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &cli_len);
    if (newsockfd < 0)
    {
        error("Error in Accepting.");
    }

    recieve_file(newsockfd);
    printf("Sending the encrypted/decrypted file back to client...\n");
    send_file(newsockfd);

    close(newsockfd);
    close(sockfd);
    return 0;
}