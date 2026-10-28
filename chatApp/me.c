#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main() {
    char message[1000];

    int userSocket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in myAddress, friendAddress;

    myAddress.sin_family = AF_INET;
    myAddress.sin_port = htons(9001);
    myAddress.sin_addr.s_addr = INADDR_ANY;

    friendAddress.sin_family = AF_INET;
    friendAddress.sin_port = htons(9002);
    friendAddress.sin_addr.s_addr = inet_addr("127.0.0.1");

    bind(userSocket, (struct sockaddr *)&myAddress, sizeof(myAddress));

    printf("User1 ready. Type exit to quit.\n\n");

    int pid = fork();

    if (pid == 0) {
        while (1) {
            int bytes = recvfrom(userSocket, message, sizeof(message) - 1, 0, NULL, NULL);
            message[bytes] = '\0';

            if (strcmp(message, "exit") == 0) {
                printf("\nUser2 left the chat.\n");
                kill(getppid(), SIGTERM);
                break;
            }

            printf("\rUser2: %s\nUser1: ", message);
            fflush(stdout);
        }
        close(userSocket);
        return 0;
    }

    while (1) {
        printf("User1: ");
        scanf(" %[^\n]", message);

        sendto(userSocket, message, strlen(message), 0, (struct sockaddr *)&friendAddress, sizeof(friendAddress));

        if (strcmp(message, "exit") == 0)
            break;
    }

    kill(pid, SIGTERM);
    close(userSocket);
    return 0;
}
