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
	    // logs dizini oluþturma
	    system("mkdir -p logs");
	
	    // Dosya ismi oluþturma: tarih, zaman ve kullanýcý adý 
	    char logFileName[50];
	    time_t t = time(NULL);
	    struct tm tm = *localtime(&t);
	    sprintf(logFileName, "logs/%d-%02d-%02d_%02d-%02d-%02d.log", 
	            tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	
	    FILE *logFile = fopen(logFileName, "a");
	    if (logFile == NULL) {
	        error("Log dosyasý oluþturulamadý");
	        exit(EXIT_FAILURE);
	    }
	     fprintf(logFile, "Username: %s - Message: %s\n", username, message);
	      fclose(logFile);
	}
	
	int simpleParityCheck(char *message) {
	    // Gelen mesajýn uzunluðunu alýp, tek mi çift mi olduðunu kontrol eder
	    int length = strlen(message);
	    return length % 2; // Örnek olarak, uzunluk tekse 1, çiftse 0 döner
	}
	
	int twoDimensionalParityCheck(char *message) {
	    // Ýki boyutlu bir parity kontrolü yapýlabilir.
	    int length = strlen(message);
	    return length % 3; // Örnek olarak, uzunluk üçe bölünüyorsa 0 döner
	}
	
	int calculateChecksum(char *message) {
	    // Checksum hesaplamak için kullanýlabilir.
	    int checksum = 0;
	    for (int i = 0; i < strlen(message); i++) {
	        checksum += (int)message[i];
	    }
	    return checksum;
	}
	
	int calculateCRC(char *message) {
	    // CRC (Cyclic Redundancy Check) hesaplamak için kullanýlýr
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
	
	    // Sunucu soketini oluþturma
	    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	    if (serverSocket < 0) {
	        error("Sunucu soketi oluþturulamadý");
	    }
	
	    memset(&serverAddr, 0, sizeof(serverAddr));
	    serverAddr.sin_family = AF_INET;
	    serverAddr.sin_addr.s_addr = INADDR_ANY;
	    serverAddr.sin_port = htons(PORT);
	
	    // Sunucu soketini baðlama
	    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
	        error("Sunucu soketi baðlanamadý");
	    }
	
	    // Baðlantýlarý dinleme
	    if (listen(serverSocket, 5) < 0) {
	        error("Baðlantý dinlenemedi");
	    }
	
	    printf("Sunucu baþlatýldý. Ýstemci baðlantýsý bekleniyor...\n");
    
      // Ýstemci baðlantýlarýný kabul etme döngüsü
		while (1) {
		    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &addrSize);
		    if (clientSocket < 0) {
		        error("Ýstemci baðlantýsý kabul edilemedi");
		    }
		
		    printf("Ýstemci baðlandý. IP Adresi: %s, Port: %d\n",
		           inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
		
		    // Mesaj alma
		    memset(buffer, 0, BUFFER_SIZE);
		    if (recv(clientSocket, buffer, BUFFER_SIZE, 0) < 0) {
		        error("Mesaj alýnamadý");
		    }
		    printf("Ýstemciden gelen mesaj: %s\n", buffer);
		    
		    // Mesajý loglama
            createLogFile("ClientUsername", buffer);
            
            // Hata kontrol tekniklerini kullanarak mesaj doðrulamasý
	        int isMessageValid = simpleParityCheck(buffer);
	        if (isMessageValid) {
	            // Doðrulama baþarýlýysa iþlem yap
	            printf("Mesaj doðrulandý.\n");
	        } else {
	            // Doðrulama baþarýsýzsa iþlem 
	            printf("Mesajda hata algýlandý.\n");
	        }
		
		    // Ýstemciye yanýt gönderme
		    const char *message = "Sunucudan gelen mesaj aldý!";
		    if (send(clientSocket, message, strlen(message), 0) < 0) {
		        error("Mesaj gönderilemedi");
		    }
		    if (send(clientSocket, &checksum, sizeof(int), 0) < 0) {
		        error("Checksum gönderilemedi");
		    }
		
		    close(clientSocket);
		    printf("Ýstemci baðlantýsý kapatýldý.\n");
		    
		    printf("Simple Parity Check: %d\n", simpleParityCheck(message));
		    printf("Two Dimensional Parity Check: %d\n", twoDimensionalParityCheck(message));
		    printf("Checksum: %d\n", calculateChecksum(message));
		    printf("CRC: %d\n", calculateCRC(message));
		}
	    close(serverSocket);
	    printf("Sunucu kapatýldý.\n");

    return 0;
}

