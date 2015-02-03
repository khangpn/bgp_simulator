#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>

using namespace std;

void bgp_send();

int main()
{
  bgp_send();
  return 0;
}
