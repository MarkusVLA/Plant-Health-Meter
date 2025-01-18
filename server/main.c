// Simple server for the ESP32 to connect to and transmit data.
// Runs only on Linux and MacOS

#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
    int server_fd, client;
    struct sockaddr_in address;
    char buffer[BUFFER_SIZE];
    FILE *fp;
    
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);
    
    bind(server_fd, (struct sockaddr *)&address, sizeof(address));
    listen(server_fd, 3);
    
    while(1) {
        client = accept(server_fd, NULL, NULL);
        read(client, buffer, BUFFER_SIZE);
        time_t now = time(NULL);
        fp = fopen("data.txt", "a");
        fprintf(fp, "%s: %s\n", ctime(&now), buffer);
        printf("%s: %s\n", ctime(&now), buffer);
        fclose(fp);
        
        close(client);
    }
}
