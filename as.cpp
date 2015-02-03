#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>
#include <thread>
#include <string> // for std::string

using namespace std;

void bgp_send();
void bgp_listen(char *port);

int main()
{
  std::thread thread1(bgp_listen, "3000");
  std::thread thread2(bgp_send);
  return 0;
}
