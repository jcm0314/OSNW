#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>  // 추가된 부분

#define MAXLINE 1024

int main(int argc, char **argv) {
    struct sockaddr_in serveraddr;
    int server_sockfd;
    char buf[MAXLINE];

    if (argc != 2) {  // 포트 번호를 인자로 받는지 확인
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return 1;
    }

    if ((server_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket error :");
        return 1;
    }

    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serveraddr.sin_port = htons(atoi(argv[1]));  // 서버 포트 번호를 명령행 인자로 받음

    // 서버에 연결을 시도한다.
    if (connect(server_sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1) {
        perror("connect error :");
        return 1;
    }

    printf("Enter a message: ");
    memset(buf, 0x00, MAXLINE);
    read(0, buf, MAXLINE);  // 키보드 입력을 기다린다.

    if (write(server_sockfd, buf, strlen(buf)) <= 0) {  // 입력 받은 데이터를 서버로 전송한다.
        perror("write error : ");
        return 1;
    }

    memset(buf, 0x00, MAXLINE);
    // 서버로부터 데이터를 읽는다.
    if (read(server_sockfd, buf, MAXLINE) <= 0) {
        perror("read error : ");
        return 1;
    }

    printf("Received from server: %s\n", buf);  // 서버로부터 받은 메시지 출력
    close(server_sockfd);
    return 0;
}
