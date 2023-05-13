#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/time.h>
#include <poll.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024

typedef struct {
    int socket;
    char name[BUFFER_SIZE];
} Client;

int main() {
    int serverSocket, newSocket, maxSockets, activity, i, valread;
    struct sockaddr_in serverAddress;
    char buffer[BUFFER_SIZE];
    Client clients[MAX_CLIENTS];
    struct pollfd fds[MAX_CLIENTS + 1];

    if ((serverSocket = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Không thể tạo socket");
        exit(EXIT_FAILURE);
    }
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(8888);
    if (bind(serverSocket, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) {
        perror("Gắn địa chỉ với socket thất bại");
        exit(EXIT_FAILURE);
    }
    if (listen(serverSocket, 5) < 0) {
        perror("Lắng nghe kết nối thất bại");
        exit(EXIT_FAILURE);
    }

    printf("Server đã khởi động. Đang lắng nghe kết nối...\n");

    maxSockets = 1;
    fds[0].fd = serverSocket;
    fds[0].events = POLLIN;

    for (i = 1; i < MAX_CLIENTS + 1; i++) {
        fds[i].fd = -1;
    }

    while (1) {
        activity = poll(fds, maxSockets + 1, -1);

        if (activity < 0) {
            perror("Hàm poll thất bại");
            exit(EXIT_FAILURE);
        }
        if (fds[0].revents & POLLIN) {
            if ((newSocket = accept(serverSocket, (struct sockaddr *)&serverAddress, (socklen_t *)&addrlen)) < 0) {
                perror("Chấp nhận kết nối mới thất bại");
                exit(EXIT_FAILURE);
            }

            printf("Kết nối mới được chấp nhận\n");
            for (i = 1; i < MAX_CLIENTS + 1; i++) {
                if (fds[i].fd == -1) {
                    fds[i].fd = newSocket;
                    fds[i].events = POLLIN;
                    break;
                }
            }
