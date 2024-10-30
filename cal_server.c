#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 3600
#define MAX_CLIENTS 3

struct cal_data {
    int left_num;
    int right_num;
    char op;
    int result;
    short int error;
};

int main(int argc, char **argv) {
    struct sockaddr_in client_addr, sock_addr;
    int listen_sockfd, client_sockfd;
    int addr_len;
    struct cal_data rdata;
    int left_num, right_num, cal_result;
    short int cal_error;

    int min_result = INT_MAX;  // 최소값 초기화
    int max_result = INT_MIN;  // 최대값 초기화
    int client_count = 0;       // 연결된 클라이언트 수

    if ((listen_sockfd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("Error ");
        return 1;
    }

    memset((void *)&sock_addr, 0x00, sizeof(sock_addr));
    sock_addr.sin_family = AF_INET;
    sock_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    sock_addr.sin_port = htons(PORT);

    if (bind(listen_sockfd, (struct sockaddr *)&sock_addr, sizeof(sock_addr)) == -1) {
        perror("Error ");
        return 1;
    }

    if (listen(listen_sockfd, 5) == -1) {
        perror("Error ");
        return 1;
    }

    while (client_count < MAX_CLIENTS) {
        addr_len = sizeof(client_addr);
        client_sockfd = accept(listen_sockfd, (struct sockaddr *)&client_addr, &addr_len);
        if (client_sockfd == -1) {
            perror("Error ");
            continue;
        }
        printf("New Client Connected: %s\n", inet_ntoa(client_addr.sin_addr));

        read(client_sockfd, (void *)&rdata, sizeof(rdata));

        cal_result = 0;
        cal_error = 0;

        left_num = ntohl(rdata.left_num);
        right_num = ntohl(rdata.right_num);

        switch (rdata.op) {
            case '+':
                cal_result = left_num + right_num;
                break;
            case '-':
                cal_result = left_num - right_num;
                break;
            case 'x':
                cal_result = left_num * right_num;
                break;
            case '/':
                if (right_num == 0) {
                    cal_error = 2;
                    break;
                }
                cal_result = left_num / right_num;
                break;
            default:
                cal_error = 1;
        }

        rdata.result = htonl(cal_result);
        rdata.error = htons(cal_error);
        printf("%d %c %d = %d\n", left_num, rdata.op, right_num, cal_result);

        // 최소값과 최대값 업데이트
        if (cal_error == 0) {  // 계산이 성공적일 경우
            if (cal_result < min_result) {
                min_result = cal_result;
            }
            if (cal_result > max_result) {
                max_result = cal_result;
            }
        }

        // 클라이언트에게 결과와 min, max 송신
        write(client_sockfd, (void *)&rdata, sizeof(rdata));
        write(client_sockfd, (void *)&min_result, sizeof(min_result));
        write(client_sockfd, (void *)&max_result, sizeof(max_result));
        
        client_count++;
        close(client_sockfd);
    }

    close(listen_sockfd);
    return 0;
}
