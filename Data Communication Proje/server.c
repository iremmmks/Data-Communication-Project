#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>
#define PORT 1234
#define BUFFER_SIZE 1024

#define MAX_MESSAGE_SIZE 1000

	void error(const char *msg) {
	    perror(msg);
	    exit(EXIT_FAILURE);
	}
	
	void createLogFile(char *username, char *message) {
	    // logs dizini olu�turma
	    system("mkdir -p logs");
	
	    // Dosya ismi olu�turma: tarih, zaman ve kullan�c� ad� 
	    char logFileName[50];
	    time_t t = time(NULL);
	    struct tm tm = *localtime(&t);
	    sprintf(logFileName, "logs/%d-%02d-%02d_%02d-%02d-%02d.log", 
	            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	    FILE *logFile = fopen(logFileName, "a");
	    if (logFile == NULL) {
	        error("Log dosyas� olu�turulamad�");
	        exit(EXIT_FAILURE);
	    }
	     fprintf(logFile, "Username: %s - Message: %s\n", username, message);
	      fclose(logFile);
	}
	
	int simpleParityCheck(char *message) {
	    // Gelen mesaj�n uzunlu�unu al�p, tek mi �ift mi oldu�unu kontrol eder
	    int length = strlen(message);
	    return length % 2; // �rnek olarak, uzunluk tekse 1, �iftse 0 d�ner
	}
	
	int twoDimensionalParityCheck(char *message) {
	    // �ki boyutlu bir parity kontrol� yap�labilir.
	    int length = strlen(message);
	    return length % 3; // �rnek olarak, uzunluk ��e b�l�n�yorsa 0 d�ner
	}
	
	int calculateChecksum(char *message) {
	    // Checksum hesaplamak i�in kullan�labilir.
	    int checksum = 0;
	    for (int i = 0; i < strlen(message); i++) {
	        checksum += (int)message[i];
	    }
	    return checksum;
	}
	
	int calculateCRC(char *message) {
	    // CRC (Cyclic Redundancy Check) hesaplamak i�in kullan�l�r
	    int crc = 0;
	    for (int i = 0; i < strlen(message); i++) {
	        crc ^= (int)message[i];
	    }
	    return crc;
	}

	int main() {
	    int serverSocket, clientSocket;
	    struct sockaddr_in serverAddr, clientAddr;
	    socklen_t addrSize = sizeof(struct sockaddr_in);
	    char buffer[BUFFER_SIZE];
	
	    // Sunucu soketini olu�turma
	    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	    if (serverSocket < 0) {
	        error("Sunucu soketi olu�turulamad�");
	    }
	
	    memset(&serverAddr, 0, sizeof(serverAddr));
	    serverAddr.sin_family = AF_INET;
	    serverAddr.sin_addr.s_addr = INADDR_ANY;
	    serverAddr.sin_port = htons(PORT);
	
	    // Sunucu soketini ba�lama
	    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
	        error("Sunucu soketi ba�lanamad�");
	    }
	
	    // Ba�lant�lar� dinleme
	    if (listen(serverSocket, 5) < 0) {
	        error("Ba�lant� dinlenemedi");
	    }
	
	    printf("Sunucu ba�lat�ld�. �stemci ba�lant�s� bekleniyor...\n");
    
      // �stemci ba�lant�lar�n� kabul etme d�ng�s�
		while (1) {
		    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
		    if (clientSocket < 0) {
		        error("�stemci ba�lant�s� kabul edilemedi");
		    }
		
		    printf("�stemci ba�land�. IP Adresi: %s, Port: %d\n",
		           inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		    // Mesaj alma
		    memset(buffer, 0, BUFFER_SIZE);
		    if (recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0) {
		        error("Mesaj al�namad�");
		    }
		    printf("�stemciden gelen mesaj: %s\n", buffer);
		    
		    // Mesaj� loglama
            createLogFile("ClientUsername", buffer);
            
            // Hata kontrol tekniklerini kullanarak mesaj do�rulamas�
	        int isMessageValid = simpleParityCheck(buffer);
	        if (isMessageValid) {
	            // Do�rulama ba�ar�l�ysa i�lem yap
	            printf("Mesaj do�ruland�.\n");
	        } else {
	            // Do�rulama ba�ar�s�zsa i�lem 
	            printf("Mesajda hata alg�land�.\n");
	        }
		
		    // �stemciye yan�t g�nderme
		    const char *message = "Sunucudan gelen mesaj ald�!";
		    if (send(clientSocket, message, strlen(message), 0) < 0) {
		        error("Mesaj g�nderilemedi");
		    }
		    if (send(clientSocket, &checksum, sizeof(int), 0) < 0) {
		        error("Checksum g�nderilemedi");
		    }
		
		    close(clientSocket);
		    printf("�stemci ba�lant�s� kapat�ld�.\n");
		    
		    printf("Simple Parity Check: %d\n", simpleParityCheck(message));
		    printf("Two Dimensional Parity Check: %d\n", twoDimensionalParityCheck(message));
		    printf("Checksum: %d\n", calculateChecksum(message));
		    printf("CRC: %d\n", calculateCRC(message));
		}
	    close(serverSocket);
	    printf("Sunucu kapat�ld�.\n");

    return 0;
}

