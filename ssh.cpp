#include <iostream>
#include <libssh/libssh.h>

using namespace std;

int main(int argc, char const *argv[])
{
  // create variables
  ssh_session session;
  int verbosity;
  int port = atoi(argv[2]);
  const char *host = argv[1];
  int connection;
  const char *password;
  ssh_channel channel;

  // check if valid input
  if (argc < 2)
  {
    cerr << "Invalid input \nUsage: host port" << endl;
    exit(0);
  }

  // set verbosity
  if (argc > 2 && argv[3] == "-v")
  {
    verbosity = SSH_LOG_PROTOCOL;
  }

  // create sesssion
  session = ssh_new();
  if (session == NULL)
  {
    cerr << "Error creating session" << endl;
    exit(0);
  }

  // configure connection
  ssh_options_set(session, SSH_OPTIONS_HOST, host);
  ssh_options_set(session, SSH_OPTIONS_LOG_VERBOSITY, &verbosity);
  ssh_options_set(session, SSH_OPTIONS_PORT, &port);

  // connect to server
  connection = ssh_connect(session);
  if (connection != SSH_OK)
  {
    cerr << "Error while connecting to " << host << " on port " << port << "\n" << ssh_get_error(session) << endl;
    ssh_free(session);
    exit(0);
  }
  cout << "Connected to " << host << " on port " << port << endl;

  // authenticate user
  password = getpass("Password: ");
  connection = ssh_userauth_password(session, NULL, password);
  if (connection != SSH_AUTH_SUCCESS)
  {
    cerr << "Authentication error: \n" << ssh_get_error(session) << endl;
    ssh_disconnect(session);
    ssh_free(session);
    exit(0);
  }
  cout << "Authentication successful\nPress any key to exit ..." << endl;

  char quit = getchar();

  // disconnect
  ssh_disconnect(session);
  ssh_free(session);

  return 0;
}
