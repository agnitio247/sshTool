#include <iostream>
#include <libssh/libssh.h>
#include "sshTool.h"

using namespace std;

// function to set private host variable
void sshSession::SetHost(const char* ssh_host)
{
  host = ssh_host;
  ssh_options_set(session, SSH_OPTIONS_HOST, host);
}

// set private port variable
void sshSession::SetPort(int ssh_port)
{
  port = ssh_port;
  ssh_options_set(session, SSH_OPTIONS_PORT, &port);
}

// get private host variable
const char* sshSession::GetHost()
{
  return host;
}

// get private port variable
int sshSession::GetPort()
{
  return port;
}

// constructor -> create ssh session and configure
sshSession::sshSession(const char* ssh_host, int ssh_port)
{
  // create sesssion
  session = ssh_new();
  if (session == NULL)
  {
    cerr << "sshSession: Error creating session" << endl;
    exit(0);
  }
  // set host and port
  this->SetHost(ssh_host);
  this->SetPort(ssh_port);
}

// constructor -> create ssh session
sshSession::sshSession()
{
  // create session
  session = ssh_new();
  if (session == NULL)
  {
    cerr << "sshSession: Error creating session" << endl;
    exit(0);
  }
}

void sshSession::Connect()
{
  // connect to server
  connection = ssh_connect(session);
  if (connection != SSH_OK)
  {
    cerr << "sshSession: Error while connecting to " << host << " on port " << port << endl;
    // free sesssion (always do before quitting)
    ssh_free(session);
  }
}

void sshSession::Authenticate(const char* ssh_password)
{
  password = ssh_password;
  connection = ssh_userauth_password(session, NULL, password);
  if (connection != SSH_AUTH_SUCCESS)
  {
    cerr << "sshSession: Authentication error: \n" << ssh_get_error(session) << endl;
    // disconnect and free before quitting
    ssh_disconnect(session);
    ssh_free(session);
    exit(0);
  }
}

void sshSession::Disconnect()
{
  ssh_disconnect(session);
}

sshSession::~sshSession()
{
  ssh_free(session);
}
