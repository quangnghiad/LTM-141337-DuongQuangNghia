#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char username[BUFFER_SIZE];
    char password[BUFFER_SIZE];
} Client;

int authenticate(Client *client) {
    char database[BUFFER_SIZE];
    char line[BUFFER_SIZE];
    char *token;

    FILE *file = fopen("database.txt", "r");
    if (file == NULL) {
        perror("Không thể mở file cơ sở dữ liệu");
        exit(EXIT_FAILURE);
    }

    while (fgets(line, sizeof(line), file)) {
        strcpy(database, line);

        token = strtok(database, " ");
        if (token != NULL && strcmp(token, client->username) == 0) {
            token = strtok(NULL, "\n");
            if (token != NULL && strcmp(token, client->password) == 0) {
                fclose(file);
                return 1; // Xác thực thành công
            }
        }
    }

    fclose(file);
    return 0; // Xác thực thất bại
}

void executeCommand(Client *client, char *command) {
    char result[BUFFER_SIZE];
    char outputFilename[BUFFER_SIZE] = "out.txt";

    snprintf(result, BUFFER_SIZE, "%s > %s", command, outputFilename);
    system(result);

    FILE *file = fopen(outputFilename, "r");
    if (file == NULL) {
        perror("Không thể mở file kết quả");
        exit(EXIT_FAILURE);
    }

    char line[BUFFER_SIZE];
    while (fgets(line, sizeof(line), file)) {
        send(client->socket, line, strlen(line), 0);
    }

    fclose(file);
}

int main() {
    int serverSocket, newSocket, maxSockets, activity, i, valread, sd, addrlen;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    Client clients[MAX_CLIENTS];
    fd_set readfds;

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);

    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress))<0) {
        perror("Không thể gắn địa chỉ với socket");
        exit(EXIT_FAILURE);
    }

    if (listen(serverSocket, 5) < 0) {
        perror("Lỗi trong quá trình lắng nghe kết nối");
        exit(EXIT_FAILURE);
    }

    addrlen = sizeof(serverAddress);
    puts("Telnet server đang chạy...");

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);
        maxSockets = serverSocket;

        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;

            if (sd > 0)
                FD_SET(sd, &readfds);


            if (sd > maxSockets)
                maxSockets = sd;
        }

  
        activity = select(maxSockets + 1, &readfds, NULL, NULL, NULL);
        if ((activity < 0) && (errno != EINTR)) {
            printf("Lỗi trong quá trình sử dụng hàm select");
        }


        if (FD_ISSET(serverSocket, &readfds)) {
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t*)&addrlen)) < 0) {
                perror("Lỗi trong quá trình chấp nhận kết nối");
                exit(EXIT_FAILURE);
            }

            char *welcomeMessage = "Chào mừng đến với Telnet Server\n";
            send(newSocket, welcomeMessage, strlen(welcomeMessage), 0);


            for (i = 0; i < MAX_CLIENTS; i++) {
                if (clients[i].socket == 0) {
                    clients[i].socket = newSocket;
                    break;
                }
            }
        }


        for (i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;

            if (FD_ISSET(sd, &readfds)) {
     
                valread = read(sd, buffer, BUFFER_SIZE);
                buffer[valread] = '\0';

                if (clients[i].username[0] == '\0') {
                    sscanf(buffer, "%[^:]:%s", clients[i].username, clients[i].password);

                    if (authenticate(&clients[i])) {
                                            char *successMessage = "Đăng nhập thành công\n";
                        send(sd, successMessage, strlen(successMessage), 0);
                    } else {
                        char *failureMessage = "Thông tin đăng nhập không hợp lệ\n";
                        send(sd, failureMessage, strlen(failureMessage), 0);

                        close(sd);
                        clients[i].socket = 0;
                    }
                } else {
                    executeCommand(&clients[i], buffer);
                }
            }
        }
    }

    return 0;
}

                       

