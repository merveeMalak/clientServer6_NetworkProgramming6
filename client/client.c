#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 2048


char name[100] = "";

void send_file(int simpleSocket){

    char fileName[100];
    FILE *fp;
    int n;
    char data[BUFFER_SIZE] = {0};
    printf("Enter your file name: ");
    fgets(fileName, 100, stdin);
    fileName[strcspn(fileName, "\n")] = 0;
    fp = fopen(fileName, "r");
    if (fp == NULL){
        perror("Error in reading file.");
        exit(1);
    }
    send(simpleSocket, fileName, sizeof(fileName), 0);
    while (fgets(data, BUFFER_SIZE, fp) != NULL){
        if (send(simpleSocket, data, sizeof(data), 0) == -1){
            perror("Error in sending file.\n");
            exit(1);
        }
        bzero(data, BUFFER_SIZE);
    }
    char result[20];
    bzero(result, sizeof(result));
    read(simpleSocket, result, sizeof(result));
    printf("%s", result);
    fclose(fp);
    
}

void login_client(int simpleSocket){
    printf("Enter your name: ");
    fgets(name, 100, stdin);
    name[strcspn(name, "\n")] = 0;
    write(simpleSocket, name, sizeof(name));
    char message[20];
    bzero(message, 20);
    read(simpleSocket, message, sizeof(message));
    printf("%s\n", message);
    return;
}
void getCommand(int simpleSocket){
    char input_operation[3];
    char result[3];
    for (;;){
        
        printf("1-login\n2-upload file\n3-exit\n");
        bzero(input_operation, sizeof(input_operation));
        printf("Choose your operation: ");
        fgets(input_operation, 3, stdin);
        input_operation[strcspn(input_operation, "\n")] = 0;
        
        if ((strcmp(input_operation, "3")) == 0){
            write(simpleSocket, input_operation, sizeof(input_operation)); //sends message to server
            printf("Client exit. Good bye\n");
            printf("********************\n");
            break;
        }
        else if ((strcmp(input_operation, "2")) == 0){
            if ((strcmp(name, "")) == 0){
                printf("You need to login first!\n");
            }
            else{
                write(simpleSocket, input_operation, sizeof(input_operation)); //sends message to server
                send_file(simpleSocket);
            }
            
        }else if((strcmp(input_operation, "1")) == 0){
            write(simpleSocket, input_operation, sizeof(input_operation)); //sends message to server
            login_client(simpleSocket);
        }else{
            printf("Invalid operation. Please enter valid operation...\n");
        }

    }
}
int main(int argc, char *argv[]) {
    int simpleSocket = 0;
    int simplePort = 1972;
    int returnStatus = 0;
    char buffer[256] = "";
    struct sockaddr_in simpleServer;

    /* create a streaming socket
    */
    simpleSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (simpleSocket == -1) {
        fprintf(stderr, "Could not create a socket!\n");
        exit(1);
    }
    
    /* set up the address structure */
    /* use the IP address argument for the server address
    */
    bzero(&simpleServer, sizeof(simpleServer));
    simpleServer.sin_family = AF_INET;
    simpleServer.sin_port = htons(simplePort);
    char *ip = "127.0.0.1"; 
    simpleServer.sin_addr.s_addr = inet_addr(ip);
    /*
    connect to the address and port with our socket
    */
    returnStatus = connect(simpleSocket, (struct sockaddr *)&simpleServer, sizeof(simpleServer));
    
    if (returnStatus == 0) {
        fprintf(stderr, "Connect successful!\n");
    }
    else {
        fprintf(stderr, "Could not connect to address!\n");
        close(simpleSocket);
        exit(1);
    }
    printf("WELCOME TO THE FILE EXCHANGE SYSTEM!\n");

    getCommand(simpleSocket);
    
    close(simpleSocket);
    return 0;
}