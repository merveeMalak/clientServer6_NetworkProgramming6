#include <stdio.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#define BUFFER_SIZE 2048
char name[100];
struct stat st = {0};

void sigchld_handler(int signo){
  while (waitpid(-1, NULL, WNOHANG) > 0);
}

void write_file(int newsock){
    int n;
    FILE *fp;
    char filename[100];
    read(newsock, filename, 100);
    char data[BUFFER_SIZE];
    strcat(name, "/");
    strcat(name, filename);
    fp = fopen(name, "w");
    while (1){
        n = recv(newsock, data, BUFFER_SIZE, 0);
        if (n <= 0){
            break;
            return;
        }
        fprintf(fp, "%s", data);
        bzero(data, BUFFER_SIZE);
        char result[20] = "File is saved\n";
        write(newsock, result, sizeof(result));
        printf("%s", result);
        fclose(fp);
    
    return;
    }
    
}

void login_client(int newsock){
    read(newsock, name, sizeof(name));
    printf("Welcome %s...\n", name);
    char message[20] = "Login successfull\n";
    if (stat(name, &st) == -1){
        mkdir(name, S_IRWXU);
    }
    write(newsock, message, sizeof(message)) >= 0;
    return;
}

void getCommand(int simpleChildSocket){
    char *result;
    for (;;){
        char comm[3];
        bzero(comm, 3);
        read(simpleChildSocket, comm, sizeof(comm)); //reads message from client
        if (strncmp("3", comm, 1) == 0){
            printf("Good bye!\n");
            break;
        }else if (strncmp("2", comm, 1) == 0){
            write_file(simpleChildSocket);
            send(simpleChildSocket, result, sizeof(result), 0);
        }else if (strncmp("1", comm, 1) == 0){
            login_client(simpleChildSocket);
        }
    }
}

int main(int argc, char *argv[])
{
    struct sockaddr_in sAddr;
    int listensock;
    int newsock;
    char buffer[25];
    int result;
    int nread;
    int pid;
    int val;
    
    listensock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    val = 1;
    result = setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));
    if (result < 0) {
        perror("server2");
        return 0;
    }

    sAddr.sin_family = AF_INET;
    sAddr.sin_port = htons(1972);
    sAddr.sin_addr.s_addr = INADDR_ANY;

    result = bind(listensock, (struct sockaddr *) &sAddr, sizeof(sAddr));
    if (result < 0) {
        perror("exserver2");
        return 0;
    }

    result = listen(listensock, 5);
    if (result < 0) {
        perror("exserver2");
        return 0;
    }

    signal(SIGCHLD, sigchld_handler);
    printf("WELCOME TO FILE EXCHANGE SYSTEM...\n");
    while (1) {
        newsock = accept(listensock, NULL ,NULL);
        if ((pid = fork()) == 0) {
            printf("Welcome new client... Please login...\n");
            close(listensock);
            getCommand(newsock);
            close(newsock);
            exit(0);
        }
        close(newsock);
    }
}
