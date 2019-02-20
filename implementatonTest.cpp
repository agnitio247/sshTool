#include <iostream>
#include "sshTool.cpp"

using namespace std;

int main(int argc, char const *argv[])
{
  sshSession *session = new sshSession("localhost", 22);
  session->Connect();
  cout << "Connected to " << session->GetHost() << " on port " << session->GetPort() << endl;
  session->Authenticate("FF8570508");
  cout << "Successfuly authenticated" << endl;
  session->Disconnect();
  cout << "Disconnected" << endl;
  delete session;
  return 0;
}
