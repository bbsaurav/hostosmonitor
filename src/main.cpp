#include<stdio.h>
#include<iostream>
#include "SSHClient.h"

using namespace std;

int main()
{
    string user, password, server, command;
    SSHClient *client = NULL;
    unsigned char buffer[10000];
    int length;

    cout << "Enter Server Address:" << endl;
    cin >> server;
    cout << "Enter UserName:" << endl;
    cin >> user;
    cout << "Enter Password:" << endl;
    cin >> password;

    client = new SSHClient(server, user, password);
    client->createSSHSession();

    getline(cin, command);
    cout << "Enter command or quit to exit" << endl;
    getline(cin, command);
    while (command != "quit") {
        if (command.length() > 0) {
            length = 0;
            cout << "Running command: " << command << endl;
            client->runCommand(command, buffer, &length);

            printf("Output length: %d\n", length);
            write(1, buffer, length);
        }
        cout << "Enter command or quit to exit" << endl;
        getline(cin, command);
    }

    delete client;
    return 0;
}