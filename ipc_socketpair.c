#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "global_types.h"

#define NUM_MESSAGES_TO_SEND 2
#define MAX_RETRIES 3

struct IPCDataStruct dataToSend[2] = { //Testing Mechanism - update to adjust tests
            {1, 1.1f, "sampleString1", type1},
            {2, 2.1f, "sampleString2", type3}
};

const char* enumToString(Types type) {
    switch(type) {
        case type1: return "type1";
        case type2: return "type2";
        case type3: return "type3";
        default: return "unknown";
    }
}

void sendData(int socket, struct IPCDataStruct* data, size_t dataSize) {
    int retries = 0;
    ssize_t totalBytesSent = 0;
    ssize_t bytesSent;

    while (totalBytesSent < dataSize) {
        bytesSent = write(socket, (char*)data + totalBytesSent, dataSize - totalBytesSent);
        if (bytesSent == -1) {
            perror("write");
            close(socket);
            exit(EXIT_FAILURE);
        }
        //Added 3 retries in case run into partial writes or padding issues
        totalBytesSent += bytesSent;
        if (totalBytesSent < dataSize && retries < MAX_RETRIES) {
            retries++;
            fprintf(stderr, "Partial write occured. Retrying - attempt %d of %d)\n", retries, MAX_RETRIES);
        } else if(totalBytesSent < dataSize) {
            fprintf(stderr, "ERROR: Incomplete/Partial write occured. All retries attempted.");
            close(socket);
            exit(EXIT_FAILURE);
        }
    }
}

void receiveData(int socket) {
    struct IPCDataStruct receivedData;
    for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
        ssize_t bytesReceived = read(socket, &receivedData, sizeof(receivedData));
        if (bytesReceived == -1) {
            perror("read");
            close(socket);
            exit(EXIT_FAILURE);
        } else if(bytesReceived == 0) {
            fprintf(stderr, "Connection closed by peer\n");
            close(socket);
            exit(EXIT_FAILURE);
        }
        const char *typeString = enumToString(receivedData.type);
        printf("\n--Next Message--\n");

        //Printing 10 diffrent versions of received data
        for (int j = 0; j < 10; ++j) {
            char combinedString[50];
            sprintf(combinedString, "%s_%d", receivedData.str, j);
            //snprintf(combinedString, sizeof(combinedString), "%s_%d", receiveData.str, j);
            printf("integer: %d, float: %.2f, string: %s, typeEnum: %s\n",
            receivedData.num + j,
            receivedData.fl + j * 0.1f,
            combinedString,
            typeString);
        }
    }
}

int main() {
    int sv[2];//socket pair file descriptors
    //AF_UNIX speciefies unix DOMAIN for address family, used for IPC on same host and allows sockets to communicate within same system using filesystem pathnames
    //SOCK_STREAM specifies stream oriented socket TYPE, provides reliable connection oriented communication simlar to TCP, ensures data is delivered in order w/o duplicates
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {//Creates pair of connected sockets 
        perror("socketpair");
        exit(EXIT_FAILURE);
    }
    
    pid_t pid = fork();//forks process to create child process. This child process acts as sender and parent process acts as receiver
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { //Must be child process - Sender
        close(sv[0]); //close unused process

        for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
            sendData(sv[1], &dataToSend[i], sizeof(dataToSend[i]));
        }
        close(sv[1]);
        exit(EXIT_SUCCESS);
    } else { //pid must be 1 aka parent process - Receiver
        close(sv[1]); //close unused process

        receiveData(sv[0]);
        close(sv[0]);
        wait(NULL); //wait for child process to finish
    }

    return 0;
}