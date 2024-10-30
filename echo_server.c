#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAXBUF 1024
#define MAXCLIENTS 3

int main(int argc, char **argv) {
    int server_sockfd, client_sockfd;
    int client_len, n;
    char buf[MAXBUF];
    struct sockaddr_in clientaddr, serveraddr;
    char combined_data[MAXBUF * MAXCLIENTS] = "";  // 수신한 데이터를 저장할 배열
    int client_count = 0;  // 연결된 클라이언트 수

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }

    client_len = sizeof(clientaddr);
    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1) {
        perror("socket error : ");
        exit(1);
    }

    memset(&serveraddr, 0x00, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
    serveraddr.sin_port = htons(atoi(argv[1]));

    if (bind(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("bind error : ");
        close(server_sockfd);
        exit(1);
    }

    if (listen(server_sockfd, 5) == -1) {
        perror("listen error : ");
        close(server_sockfd);
        exit(1);
    }

    while (client_count < MAXCLIENTS) {
        client_sockfd = accept(server_sockfd, (struct sockaddr *)&clientaddr, &client_len);
        if (client_sockfd < 0) {
            perror("accept error : ");
            continue;
        }

        printf("New Client Connect: %s\n", inet_ntoa(clientaddr.sin_addr));
        memset(buf, 0x00, MAXBUF);
        
        // 클라이언트로부터 문자열 수신
        if ((n = read(client_sockfd, buf, MAXBUF)) <= 0) {
            close(client_sockfd);
            continue;
        }

        // 수신한 데이터를 combined_data에 연결
        strncat(combined_data, buf, sizeof(combined_data) - strlen(combined_data) - 1);
        client_count++;

        // 모든 클라이언트에게 연결된 데이터 송신
        write(client_sockfd, combined_data, strlen(combined_data));
        close(client_sockfd);
    }

    close(server_sockfd);
    return 0;
}
