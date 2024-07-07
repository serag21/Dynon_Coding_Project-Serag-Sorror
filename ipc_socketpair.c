#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "global_types.h"

#define NUM_MESSAGES_TO_SEND 2
#define MAX_RETRIES 3

int main() {
    //int beginTime = clock();

    int sv[2];//socket pair file descriptors
    if(socketpair(AF_UNIX, SOCK_STREAM, 0, sv) == -1) {//Creates pair of connected sockets 
        perror("socketpair");
        exit(EXIT_FAILURE);
    }//AF_UNIX = domain
    //AF_UNIX speciefies unix DOMAIN for address family, used for IPC on same host and allows sockets to communicate within same system using filesystem pathnames
    //SOCK_STREAM specifies stream oriented socket TYPE, provides reliable connection oriented communication simlar to TCP, ensures data is delivered in order w/o duplicates
    //0 = protocol
    //sv = file descriptors used in referencing the new sockets
    //overall creates an unnamed pair of ocnnected sockets in specified DOMAIN, of the specified TYPE, and using optionally specificied PROTOCOL
    pid_t pid = fork();//forks process to create child process. This child p;rocess acts as sender and parent process acts as receiver
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    } else if (pid == 0) { //Must be child process - Sender
        close(sv[0]); //close unused process

        struct IPCDataStruct dataToSend[2] = {
            {1, 1.1f, "sampleString1", type1}, {2, 2.1f, "sampleString2", type2}
        };

        // for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
        //     ssize_t bytes_sent = write(sv[1], &dataToSend[i], sizeof(dataToSend[i])); //was facing partial write issues where nhot all bytes of struct were written to socket,
        //                                                     //could be due to various reasons such as buffer size or interruptions in communication.
        //                                                     //SOLUTION: Added check after each 'write' operation to verify exact number of bytes from each array index in 'dataToSend' was successfully written

        //                                                     //ISSUE ALSO COULD HAVE BEEN EITHER 1. SIZE MISMATCH, 2. ALIGNMENT AND PADDING CAUSING MISINTERPRETATION, 3. READING LOGIC(READ CORRECT NUMBER OF BYTES)
        //     if (bytes_sent == -1) {
        //         perror("write");
        //         close(sv[1]);
        //         exit(EXIT_FAILURE);
        //     } else if(bytes_sent != sizeof(dataToSend[i])) {
        //         fprintf(stderr, "ERROR: Incomplete/Partial write occured\n");
        //         close(sv[1]);
        //         exit(EXIT_FAILURE);
        //     }

for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
            int retries = 0;
            ssize_t totalBytesSent = 0;
            ssize_t bytesSent;

            while (totalBytesSent < sizeof(dataToSend[i])) {
                bytesSent = write(sv[1], ((char *)&dataToSend[i]) + totalBytesSent, sizeof(dataToSend[i]) - totalBytesSent);
                if (bytesSent == -1) {
                    perror("write");
                    close(sv[1]);
                    exit(EXIT_FAILURE);
                }
                //Added 3 retries in case run into partial writes or padding issues
                totalBytesSent += bytesSent;
                if (totalBytesSent < sizeof(dataToSend[i]) && retries < MAX_RETRIES) {
                    retries++;
                    fprintf(stderr, "Partial write occured. Retrying - attempt %d of %d)\n", retries, MAX_RETRIES);
                } else if(totalBytesSent < sizeof(dataToSend[i])) {
                    fprintf(stderr, "ERROR: Incomplete/Partial write occured. All retries attempted.");
                    close(sv[1]);
                    exit(EXIT_FAILURE);
                }
            }
        }
        //}
        

        close(sv[1]);
        exit(EXIT_SUCCESS);
    } else { //pid must be 1 aka parent process - Receiver
        close(sv[1]); //close unused process

    //     struct DataStruct receivedData[NUM_MESSAGES_TO_SEND];


    //     for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
    //         ssize_t bytes_received = read(sv[0], &receivedData[i], sizeof(receivedData[i]));
    //         if (bytes_received == -1) {
    //             perror("read");
    //             close(sv[0]);
    //             exit(EXIT_FAILURE);
    //         } else if (bytes_received != sizeof(receivedData[i])) {
    //             fprintf(stderr, "Incomplete/Partial read occured");
    //             close(sv[0]);
    //             exit(EXIT_FAILURE);
    //         }
            
    //         const char *typeString; //CHANGE NAME
    //         switch (receivedData[i].type) {
    //             case type1: typeString = "type1"; break;
    //             case type2: typeString = "type2"; break;
    //             case type3: typeString = "type3"; break;
    //             default: typeString = "unknown"; break;
    //         }
    //         printf("\n--Next Message--\n");
    //         //Print 10 diffrent versions of received data
    //         for (int j = 0; j < 10; ++j) {
    //             char combinedString[25];
    //             sprintf(combinedString, "%s_%d", receivedData[i].str, j);
    //             printf("integer: %d, float: %.2f, string: %s, type: %s\n",
    //             receivedData[i].num + j,
    //             receivedData[i].fl + j * 0.1f,
    //             combinedString,
    //             typeString);
    //         }
            
    //    }

    struct IPCDataStruct receivedData;
    for (int i = 0; i < NUM_MESSAGES_TO_SEND; ++i) {
        ssize_t bytesReceived = read(sv[0], &receivedData, sizeof(receivedData));
        if (bytesReceived == -1) {
            perror("read");
            close(sv[0]);
            exit(EXIT_FAILURE);
        } else if(bytesReceived == 0) {
            fprintf(stderr, "Connection closed by peer\n");
            close(sv[0]);
            exit(EXIT_FAILURE);
        }
        const char *typeString; //CHANGE NAME
            switch (receivedData.type) {
                case type1: typeString = "type1"; break;
                case type2: typeString = "type2"; break;
                case type3: typeString = "type3"; break;
                default: typeString = "unknown"; break;
            }
            printf("\n--Next Message--\n");
            //Print 10 diffrent versions of received data
            for (int j = 0; j < 10; ++j) {
                char combinedString[25];
                sprintf(combinedString, "%s_%d", receivedData.str, j);
                printf("integer: %d, float: %.2f, string: %s, type: %s\n",
                receivedData.num + j,
                receivedData.fl + j * 0.1f,
                combinedString,
                typeString);
            }
    }

        close(sv[0]);
        wait(NULL); //wait for child process to finish
    }
    // int end = clock();
    // printf("execution time was: %d ticks or %.2f seconds", end - beginTime, ((float)end - beginTime)/CLOCKS_PER_SEC);
    return 0;
}