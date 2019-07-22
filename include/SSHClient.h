#include<libssh/libssh.h>

using namespace std;

class SSHClient
{
    private:
    ssh_session session = NULL;
    string serverAddr;
    string userName;
    string passWord;
    
    SSHClient();
    int verify_knownhost(ssh_session session);

    public:
    SSHClient(string server, string user, string passwd);
    ~SSHClient();
    int createSSHSession();
    int runCommand(string command, unsigned char *buffer, int *bufLen);
};
