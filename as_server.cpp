#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>

using namespace std;

void bgp_listen();

int main()
{
  bgp_listen();
  return 0;
}
