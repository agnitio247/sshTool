#pragma once

#include <iostream>
#include <libssh/libssh.h>

using namespace std;

class sshSession
{
private:
  ssh_session session;
  int connection, port, rc, nbytes;
  const char *password, *host;
  ssh_channel channel;
  char buffer[256];

public:
  void SetHost(const char* ssh_host);
  void SetPort(int ssh_port);
  const char* GetHost();
  int GetPort();

  sshSession(const char* ssh_host, int ssh_port);
  sshSession();

  void Connect();
  void Authenticate(const char* ssh_password);
  void Disconnect();

  ~sshSession();

  void Execute(const char *command);
};
