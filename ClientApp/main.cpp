#include <iostream>
#include <signal.h>
#include <fcntl.h>
#include "cmake-build-debug/TCPClient.h"

TCPClient tcp;

void sig_exit(int s)
{
    tcp.exit();
    exit(0);
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_exit);



    tcp.setup("127.0.0.1",8888);
    while(true)
    {
        //srand(time(NULL));
        //tcp.Send(to_string(rand()%25000));
        string rec = tcp.receive();
        if( rec != "" )
        {
            cout << "\nServer Response:" << rec << endl;
        }
        //sleep(1);
        /**string s;
        getline(cin, s);
        tcp.Send(s);*/
        char buffer[512];
        int point = 0;

    }
    return 0;
}