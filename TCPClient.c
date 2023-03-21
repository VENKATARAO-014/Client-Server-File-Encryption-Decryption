//Program-->TCP Client for file encryption and decryption..!!

#include <netdb.h>
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

//function for sending file..!
void send_file(int sockfd)
{
    char buffer[255];
    bzero(buffer, 255);

    int words = 0;
    FILE *f;
    char c;
    char filename[25];
    printf("Enter the name of file: ");
    gets(filename);
    //Open the file..!
    f = fopen(filename, "r");
    while ((c = getc(f)) != EOF)
    {
        fscanf(f, "%s", buffer);
        if (c == ' ' || c == '\t')
            words++;
    }
    write(sockfd, &words, sizeof(int));
    rewind(f);

    printf("Enter:\n 1 to encrypt the file.\n 2 to decrypt the file.\n");
    int enc;
    scanf("%d", &enc);

    write(sockfd, &enc, sizeof(int));

    char ch;
    do
    {
        fscanf(f, "%s", buffer);
        write(sockfd, buffer, 255);
        ch = fgetc(f);
    } while (ch != EOF);

    fclose(f);

    printf("The file has been successfully sent.\n");
}

//Function for recieved file..!
void recieve_file(int sockfd)
{

    char buffer[255];
    printf("Recieving the encrypted/decrypted file....\n");

    FILE *fr;
    fr = fopen("file_recieved.txt", "w");
    int words, curr = 0;

    read(sockfd, &words, sizeof(int));
    printf("words: %d\n", words);

    while (curr != words)
    {
        read(sockfd, buffer, 255);
        fprintf(fr, "%s ", buffer);
        curr++;
    }

    fclose(fr);

    printf("The file has been recieved successfully [File Name: "
           "file_recieved.txt] \n");
}

// Driver code
int main(int argc, char *argv[])
{

    if (argc < 3)
    {
        fprintf(stderr, "Usage %s hostname port\n", argv[0]);
        exit(1);
    }

    int sockfd, portno, n;
    struct sockaddr_in serv_addr, cli_addr;
    struct hostent *server;

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        error("Error opening Socket.");
    }

    server = gethostbyname(argv[1]);
    if (server == NULL)
    {
        error("No such host.");
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(portno);

    //socket connection
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        error("Connection Failed.");
    }

    send_file(sockfd);
    recieve_file(sockfd);

    close(sockfd);
    return 0;
}