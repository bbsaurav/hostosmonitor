#include<libssh/libssh.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<string>
#include<stdio.h>
#include "SSHClient.h"

using namespace std;

SSHClient::SSHClient(string server, string user, string passwd) :
    serverAddr(server), userName(user), passWord(passwd)
{

}

int SSHClient::verify_knownhost(ssh_session session)
{
    enum ssh_known_hosts_e state;
    unsigned char *hash = NULL;
    ssh_key srv_pubkey = NULL;
    size_t hlen;
    char buf[10];
    char *hexa;
    int cmp;
    int rc;
    rc = ssh_get_server_publickey(session, &srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    rc = ssh_get_publickey_hash(srv_pubkey,
                                SSH_PUBLICKEY_HASH_SHA1,
                                &hash,
                                &hlen);
    ssh_key_free(srv_pubkey);
    if (rc < 0) {
        return -1;
    }
    state = ssh_session_is_known_server(session);
    switch (state) {
        case SSH_KNOWN_HOSTS_OK:
            /* OK */
            break;
        case SSH_KNOWN_HOSTS_CHANGED:
            printf("Host key for server changed: it is now:\n");
            ssh_print_hexa("Public key hash", hash, hlen);
            printf("For security reasons, connection will be stopped\n");
            ssh_clean_pubkey_hash(&hash);
            return -1;
        case SSH_KNOWN_HOSTS_OTHER:
            printf("The host key for this server was not found but an other"
                    "type of key exists.\n");
            printf("An attacker might change the default server key to"
                    "confuse your client into thinking the key does not exist\n");
            ssh_clean_pubkey_hash(&hash);
            return -1;
        case SSH_KNOWN_HOSTS_NOT_FOUND:
            printf("Could not find known host file.\n");
            printf("If you accept the host key here, the file will be"
                    "automatically created.\n");
            /* FALL THROUGH to SSH_SERVER_NOT_KNOWN behavior */
        case SSH_KNOWN_HOSTS_UNKNOWN:
            hexa = ssh_get_hexa(hash, hlen);
            printf("Public key hash: %s\n", hexa);
            ssh_string_free_char(hexa);
            ssh_clean_pubkey_hash(&hash);
            
            /* Add server to trusted host */
            rc = ssh_session_update_known_hosts(session);
            if (rc < 0) {
                printf("Error %s\n", strerror(errno));
                return -1;
            }
            break;
        case SSH_KNOWN_HOSTS_ERROR:
            printf("Error %s", ssh_get_error(session));
            ssh_clean_pubkey_hash(&hash);
            return -1;
    }
    ssh_clean_pubkey_hash(&hash);
    return 0;
}

int SSHClient::createSSHSession()
{
    int ret;
    session = ssh_new();
    if (NULL == session) {
        printf("Failed to create ssh session\n");
        return -1;
    }

    /* Set SSH options */
    ssh_options_set(session, SSH_OPTIONS_HOST, serverAddr.c_str());
    ssh_options_set(session, SSH_OPTIONS_USER, userName.c_str());

    ret = ssh_connect(session);
    if (ret != SSH_OK) {
        printf("Failed to connect to server: [%s]\n", ssh_get_error(session));
        ssh_free(session);
        return -1;
    }

    ret = verify_knownhost(session);
    if (ret < 0) {
        printf("Failed to verify knownhost");
        ssh_disconnect(session);
        ssh_free(session);
        return -1;
    }

    ret = ssh_userauth_password(session, NULL, passWord.c_str());
    if (ret != SSH_AUTH_SUCCESS) {
        printf("Error authenticating with password: %s\n", ssh_get_error(session));
        ssh_disconnect(session);
        ssh_free(session);
        return -1;
    }

    printf("SSH Session created successfully\n");
    return 0;
}

SSHClient::~SSHClient()
{
    ssh_disconnect(session);
    ssh_free(session);
    printf("SSH Session destroyed successfully\n");
}

int SSHClient::runCommand(string command, unsigned char *buffer, int *bufLen)
{
    ssh_channel channel;
    int rc;
    char buf[10000];
    int nbytes;
    channel = ssh_channel_new(session);
    if (channel == NULL)
        return -1;
    rc = ssh_channel_open_session(channel);
    if (rc != SSH_OK)
    {
        ssh_channel_free(channel);
        return -1;
    }
    rc = ssh_channel_request_exec(channel, command.c_str());
    if (rc != SSH_OK)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return -1;
    }

    /* Initialize bufLen with 0 */
    *bufLen = 0;

    nbytes = ssh_channel_read(channel, buf, sizeof(buf), 0);
    while (nbytes > 0)
    {
        memcpy(buffer + *bufLen, buf, nbytes);
        *bufLen += nbytes;

        nbytes = ssh_channel_read(channel, buf, sizeof(buf), 0);
    }

    if (nbytes < 0)
    {
        ssh_channel_close(channel);
        ssh_channel_free(channel);
        return -1;
    }

    ssh_channel_send_eof(channel);
    ssh_channel_close(channel);
    ssh_channel_free(channel);

    return 0;
}