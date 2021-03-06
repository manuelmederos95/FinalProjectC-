#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <arpa/inet.h>
#include <vector>   //close
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros
#include <fstream>  // For the log file
#include <sstream>


#define TRUE   1
#define FALSE  0
#define PORT 8888

static void concatint(char tab[], int position, int valuetoadd);
static void sendhistoric(int sd);
static void sendprivately(int sd[], char tab[], int from, char message[]);

int main(int argc , char *argv[])
{
    int opt = TRUE;
    int master_socket , addrlen , new_socket , client_socket[30] ,
            max_clients = 30 , activity, i , valread , sd;
    int max_sd;
    struct sockaddr_in address;

    char messageforId[] = "Connected as Client 0 \n";
    char clientId[] = "Client 0 :";
    char connectedWarning[] = "Client 0  is now connected\n";
    char disconnectedWarning[] = "Client 0  is now disconnected\n";
    char buffer[1025];  //data buffer of 1K
    std::ofstream logs;


    //set of socket descriptors
    fd_set readfds;

    //a message
    char message[] = "Final Project C++ \r\n";


    //initialise all client_socket[] to 0 so not checked
    for (i = 0; i < max_clients; i++)
    {
        client_socket[i] = 0;
    }

    //create a master socket
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    //set master socket to allow multiple connections ,
    //this is just a good habit, it will work without this
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt,
                   sizeof(opt)) < 0 )
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    //type of socket created
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons( PORT );

    //bind the socket to localhost port 8888
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }
    printf("Listener on port %d \n", PORT);

    //try to specify maximum of 3 pending connections for the master socket
    if (listen(master_socket, 3) < 0)
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    //accept the incoming connection
    addrlen = sizeof(address);
    puts("Waiting for connections ...");

    while(TRUE)
    {
        //clear the socket set
        FD_ZERO(&readfds);

        //add master socket to set
        FD_SET(master_socket, &readfds);
        max_sd = master_socket;

        //add child sockets to set
        for ( i = 0 ; i < max_clients ; i++)
        {
            //socket descriptor
            sd = client_socket[i];

            //if valid socket descriptor then add to read list
            if(sd > 0)
                FD_SET( sd , &readfds);

            //highest file descriptor number, need it for the select function
            if(sd > max_sd)
                max_sd = sd;
        }

        //wait for an activity on one of the sockets , timeout is NULL ,
        //so wait indefinitely
        activity = select( max_sd + 1 , &readfds , NULL , NULL , NULL);

        if ((activity < 0) && (errno!=EINTR))
        {
            printf("select error");
        }

        //If something happened on the master socket ,
        //then its an incoming connection
        if (FD_ISSET(master_socket, &readfds))
        {
            if ((new_socket = accept(master_socket,
                                     (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)
            {
                perror("accept");
                exit(EXIT_FAILURE);
            }

            //inform user of socket number - used in send and receive commands
            printf("New connection , socket fd is %d , ip is : %s , port : %d\n" , new_socket , inet_ntoa(address.sin_addr) , ntohs
                    (address.sin_port));

            //send new connection greeting message
            if( send(new_socket, message, strlen(message), 0) != strlen(message) )
            {
                perror("send");
            }

            puts("Welcome message sent successfully");

            //add new socket to array of sockets
            for (int i = 0; i < max_clients; i++)
            {
                //if position is empty
                if( client_socket[i] == 0 )
                {
                    client_socket[i] = new_socket;
                    printf("Adding to list of sockets as %d\n" , i);

                    concatint(messageforId,20,i);
                    concatint(connectedWarning,7,i);
                    if( send(new_socket, messageforId, strlen(messageforId), 0) != strlen(messageforId))
                    {
                        perror("send");
                    }

                    //we warn the others clients of the new connection
                    for(int j = 0; j < max_clients; j++){
                        if(j != i && client_socket[j] != 0) {
                            if (send(client_socket[j], connectedWarning, strlen(connectedWarning), 0) !=
                                strlen(connectedWarning)) {
                                perror("send");
                            }
                        }
                    }

                    break;
                }
            }
        }

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)
        {
            char messagetosend[1025] = "";
            sd = client_socket[i];

            if (FD_ISSET( sd , &readfds))
            {
                //Check if it was for closing , and also read the
                //incoming message
                if ((valread = read( sd , buffer, 1024)) == 0)
                {
                    //Somebody disconnected , get his details and print
                    getpeername(sd , (struct sockaddr*)&address , \
                        (socklen_t*)&addrlen);
                    printf("Host disconnected , ip %s , port %d \n" ,
                           inet_ntoa(address.sin_addr) , ntohs(address.sin_port));

                    //Close the socket and mark as 0 in list for reuse
                    close( sd );
                    client_socket[i] = 0;
                    concatint(disconnectedWarning,7,i);
                    //Inform the others clients about the disconnection
                    for (int k = 0; k < max_clients; k++) {
                        if(k != i){
                            sd = client_socket[k];
                            send(sd , disconnectedWarning , strlen(disconnectedWarning) , 0 );
                        }
                    }
                }

                    //Echo back the message that came in to each client
                    // (no to the client that sent the message)
                else
                {

                    concatint(clientId,7,i);
                    char hist[] = "/history";
                    char privatetoclient[] = "@Client";
                    char privatechat[50];
                    strncpy(privatechat,buffer,7);
                    if(strstr(buffer, hist) != NULL){ //Call the history function
                        sendhistoric(sd);
                    }
                    else if(strstr(privatetoclient, privatechat) != NULL){
                        buffer[valread] = '\0';
                        strcat(messagetosend, clientId);
                        strcat(messagetosend, buffer);
                        sendprivately(client_socket,buffer,i,messagetosend);
                    }
                    else {
                        //set the string terminating NULL byte on the end
                        //of the data read
                        buffer[valread] = '\0';
                        strcat(messagetosend, clientId);
                        strcat(messagetosend, buffer);
                        logs.open("logs.txt", std::ofstream::out | std::ofstream::app);
                        logs << messagetosend;// write in the log file
                        logs.close();

                        for (int j = 0; j < max_clients; j++) {
                            if (j != i) {
                                sd = client_socket[j];
                                send(sd, messagetosend, strlen(messagetosend), 0);
                            }
                        }
                    }
                }
            }
        }
    }

    return 0;
}

static void concatint(char tab[], int position, int valuetoadd){
    if(valuetoadd <= 8)
        tab[position] = (valuetoadd + 1) + '0';
    else if ((valuetoadd > 8) &&  (valuetoadd < 19)){
        tab[position] = '1';
        tab[position + 1] = (valuetoadd + 1 - 10) + '0';
    }
    else{
        tab[position] = '2';
        tab[position + 1] = '0';
    }
}

static void sendhistoric(int sd){
    std::vector<std::string> list;
    std::ifstream logs("logs.txt");
    //logs.open("logs;txt",std::ifstream::in);
    std::string line;


    while(logs && std::getline(logs, line)){
        char *c = const_cast<char*>(line.c_str());
        strcat(c,"\n");
        if(line.length() == 0) continue;
        send(sd , c , strlen(c) , 0 );
    }
}

// The syntax accepted is:
// @Client 0 Message
// @Client 00 Message
static void sendprivately(int sd[], char tab[], int from, char message[]){
    int clientNum;
    char errorMessage[] = "Syntax error\n";
    char noClientError[] = "Client no exist\n";
    if((tab[7] != ' ') || (tab[8] <= '0') || tab[8] > '9'){
        send(sd[from] , errorMessage , strlen(errorMessage) , 0 );
    }
    else{
        if(tab[9] == ' ') {
            clientNum = tab[8] - '0';
            if(sd[clientNum - 1] == 0) send(sd[from] , noClientError , strlen(noClientError) , 0 );
            else{
                send(sd[clientNum - 1] , message , strlen(message) , 0 );
            }
        }

        else if(tab[10] >= '0' && tab[10] <= '9'){
            clientNum = 10 + (tab[8] - '0');
            if(sd[clientNum - 1] == 0) send(sd[from] , noClientError , strlen(noClientError) , 0 );
            else{
                send(sd[clientNum - 1] , message , strlen(message) , 0 );
            }
        }
    }
}