#include <iostream>
#include <signal.h>
#include <future>
#include <pthread.h>
#include <chrono>
#include <fcntl.h>
#include "cmake-build-debug/TCPClient.h"
#include "cmake-build-debug/clavier_non_bloquant.h"


TCPClient tcp;

void sig_exit(int s)
{
    tcp.exit();
    exit(0);
}

static string getAnswer(){
    string answer;
    cin >> answer;
    return answer;
}

int main(int argc, char *argv[])
{
    signal(SIGINT, sig_exit);


    tcp.setup("127.0.0.1",8888);
    clavier_non_bloquant::lancer();
    while(true)
    {


        //srand(time(NULL));
        //tcp.Send(to_string(rand()%25000));
        string rec = tcp.receive();
        if( rec != "" )
        {
            //cout << "\nServer Response:" << rec ;
            cout << rec;
        }
        //sleep(1);
        //string s;
        //getline(cin, s);
        //tcp.Send(s);

        /**
        int timeout = 0;
        string answer;

        future<string> future = async(getAnswer);
        if (future.wait_for(chrono::seconds(timeout)) == future_status::ready)
            answer = future.get();
        cout << answer << endl;
        tcp.Send(answer);
         */
        int i = 0;
        string str;
        clavier_non_bloquant::lireLigne(str);
        i++;
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
        tcp.Send(str);

    }
    return 0;
}