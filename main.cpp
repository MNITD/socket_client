#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <vector>

void error(const std::string &m){
    std::string message("[client] ERROR "+m);
    perror(message.c_str());
    exit(1);
}
void log(const std::string &m){
    printf("[client] %s \n", m.c_str());
}
bool check_ops(const std::string &o, const std::vector<std::string> v){
    for (int i = 0; i < v.size(); i++){
        if(o.compare(v.at((unsigned)i))==0)
            return  true;
    }
    return false;
}

int main(int argc, char *argv[]) {
    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    std::vector <std::string> ops = {"d", "t","h", "m" };

    char buffer[256];

    if (argc < 3) {
        error("requires hostname and command");
    }
    if(!check_ops(argv[2], ops)){
        fprintf(stderr, "[client] ERROR, undefined command\n");
        exit(1);
    }

    /* Getting portno from args*/
//    if(argc > 2)
//        portno = atoi(argv[2]); // converts string to int
//    else
    portno = 5002; // default port


    /* Create a socket file descriptor */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("opening socket");
    }

    server = gethostbyname(argv[1]);

    if (server == NULL) {
        error("no such host");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, (size_t)server->h_length);
    serv_addr.sin_port = htons(portno);

    /* Connect to the server */
    if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
        error("connecting to server");
    else
        log("Has connected to server");

    /* Now ask for a message from the user, this message
       * will be read by server
    */

//    printf("[client] enter the message: ");
    bzero(buffer,256);
//    fgets(buffer,255,stdin);

    strcpy(buffer, argv[2]);

    for(int i = 3; i < argc; i++){
        strcat(buffer, " ");
        strcat (buffer, argv[i]);
    }

    /* Send message to the server */
    if (write(sockfd, buffer, strlen(buffer)) < 0) {
        error("writing to socket");
    }

    /* Read server response */
    bzero(buffer,256);

    if (read(sockfd, buffer, 255) < 0) {
        error("reading from socket");
    }

    printf("[server] %s\n", buffer);
    return 0;
}