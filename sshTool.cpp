#include <iostream>
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
  // free session when deleting class instence
  ssh_free(session);
}

void sshSession::Execute(const char *command)
{
  // create new channel
  channel = ssh_channel_new(session);
  if (channel == NULL)
  {
    cerr << "sshSession: Error creating channel" << endl;
    ssh_disconnect(session);
    ssh_free(session);
    exit(0);
  }

  // create new session using new channel
  rc = ssh_channel_open_session(channel);
  if (rc != SSH_OK)
  {
    cerr << "sshSession: Error opening session using channel" << endl;
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    exit(0);
  }

  // execute command
  rc = ssh_channel_request_exec(channel, command);
  if (rc != SSH_OK)
  {
    cerr << "sshSession: Error trying to execute command: " << command << " on remote host" << endl;
    ssh_channel_close(channel);
    ssh_channel_send_eof(channel);
    ssh_channel_free(channel);
    ssh_disconnect(session);
    ssh_free(session);
    exit(0);
  }

  // if something has to be printed to the screen (command output)
  nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  while (nbytes > 0)
  {
    if (fwrite(buffer, 1, nbytes, stdout) != nbytes)
    {
      cout << "sshSession: Error reading command output on remote host" << endl;
      ssh_channel_close(channel);
      ssh_channel_send_eof(channel);
      ssh_channel_free(channel);
      ssh_disconnect(session);
      ssh_free(session);
      exit(0);
    }
    nbytes = ssh_channel_read(channel, buffer, sizeof(buffer), 0);
  }

  // close channel
  ssh_channel_close(channel);
  ssh_channel_send_eof(channel);
  ssh_channel_free(channel);
}
