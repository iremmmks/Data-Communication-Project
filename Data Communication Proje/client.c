#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 1234
#define BUFFER_SIZE 1024

void error(const char *msg) {
    error(msg);
    exit(EXIT_FAILURE);
}

int main() {
    int clientSocket;
    struct sockaddr_in serverAddr;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];

    // Ýstemci soketini oluþturma
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        error("Ýstemci soketi oluþturulamadý");
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Sunucu adresi
    serverAddr.sin_port = htons(PORT);

    // Sunucuya baðlanma
    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        error("Sunucuya baðlanýlamadý");
    }

    printf("Sunucuya baðlanýldý.\n");

    // Mesaj gönderme
    const char *messageToSend = "Merhaba!";
    if (send(clientSocket, messageToSend, strlen(messageToSend), 0) < 0) {
        error("Mesaj gönderilemedi");
    }

    // Sunucudan gelen yanýtý alma
    memset(buffer, 0, BUFFER_SIZE);
    if (recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0) {
        error("Cevap alýnamadý");
    }
    printf("Sunucudan gelen yanýt: %s\n", buffer);

    close(clientSocket);
    printf("Ýstemci kapatýldý.\n");

    return 0;
}

