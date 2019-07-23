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

    startServer(client);

    delete client;
    return 0;
}