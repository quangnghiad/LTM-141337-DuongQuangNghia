#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 5000
#define BUFFER_SIZE 1024

int main(int argc, char const *argv[]) {
    int server_fd, client_fd, valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};


    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

   
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

 
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }
    printf("Listening on port %d\n", PORT);

    
    if ((client_fd = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        valread = read(client_fd, buffer, BUFFER_SIZE);
        if (valread <= 0) {
            break;
        }

  
        char *start = buffer;
        char *end = buffer;
        while (*end != '\0') {
            if (*end == '0' || *end == '1' || *end == '2' || *end == '3' || *end == '4' || *end == '5' || *end == '6' || *end == '7' || *end == '8' || *end == '9') {
                end++;
            } else {
                int length = end - start;
                char output[length + 1];
                memcpy(output, start, length);
                output[length] = '\0';
                printf("%s", output);
                start = end + 1;
                end = start;
            }
        }
        int length = end - start;
        char output[length + 1];
        memcpy(output, start, length);
        output[length] = '\0';
        printf("%s", output);
    }

    close(client_fd);
    close(server_fd);
    return 0;
}
