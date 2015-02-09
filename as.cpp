#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>
#include <thread>
#include <string> // for std::string
#include <fstream>

using namespace std;

void bgp_send(char *port);
void bgp_listen(char *port);

int main()
{
  /* Setup AS server */

  // Read configuration file
  string port;
  ifstream config_file;
  config_file.open("as_configs");
  getline(config_file, port);
  config_file.close();

  // Convert string to char[]
  char listen_port[10];
  strcpy(listen_port, port.c_str());

  thread thread1(bgp_listen, listen_port);
  thread1.detach();
  /* ========== END ========== */

  /* Setup AS links */

  // Read configuration file
  string link;
  ifstream links_file;
  config_file.open("as_links.csv");
  while(getline(config_file, link))
  {
    // Convert string to char[]
    char link_port[10];
    strcpy(link_port, link.c_str());

    thread thread2(bgp_send, link_port);
    thread2.join();
  }
  config_file.close();

  /* ========== END ========== */

  return 0;
}
