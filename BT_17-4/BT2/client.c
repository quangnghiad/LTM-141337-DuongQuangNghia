#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 8888

int main(int argc, char const *argv[]) {
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Socket creation error \n");
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);


    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        printf("\nConnection Failed \n");
        return -1;
    }

    FILE *file;
    char filename[100];
    printf("Enter file name: ");
    scanf("%s", filename);

    file = fopen(filename, "r");
    if (file == NULL) {
        printf("\nFile not found\n");
        return -1;
    }

    fseek(file, 0L, SEEK_END);
    int filesize = ftell(file);
    rewind(file);

    char *filebuffer = (char *)malloc(filesize);
    if (filebuffer == NULL) {
        printf("\nError in malloc()\n");
        return -1;
    }

    fread(filebuffer, filesize, 1, file);

    if (send(sock, filebuffer, filesize, 0) < 0) {
        printf("\nFailed to send data\n");
        return -1;
    }
    printf("Data sent to server.\n");
    fclose(file);
    free(filebuffer);

    return 0;
}
