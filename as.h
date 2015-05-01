#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>
#include <thread>
#include <fstream>
#include <string> // for std::string
#include <sstream> // for std::stringstream
#include <map> // for std::map
#include <stdlib.h> // for std::sleep
using namespace std;

class As {
  string port;
  int name;
  map<string, string> neighbours;
  const int HEADER_LENGTH = 18;

  //unsigned char * serialize_int(unsigned char *buffer, int value);
  unsigned char * serialize_char(unsigned char *buffer, char value, int *size);

  // Total: 18 octets
  struct header {
    unsigned char marker [16];
    unsigned char length = 18; //This should be 2 octets. Let's assume it 1octet atm.
    /*
      message types:
      1 - OPEN
      2 - UPDATE
      3 - NOTIFICATION
      4 - KEEPALIVE
    */
    unsigned char type;
  } header;
  unsigned char * serialize_HEADER(unsigned char * buffer, struct header *value, int *size);

  // Total: 7 octets
  struct open_msg {
    unsigned char version = 4;
    unsigned char my_as; //should be 2 octets
    unsigned char holdtime = 3; //Should be 2 octets
    unsigned char bgp_identifier [4] = {192, 168, 0 ,1}; // Hardcode AS IP
    //Atm, let's ignore optional parameters
  } open_msg;
  unsigned char * serialize_OPEN(unsigned char * buffer, struct open_msg *value, int *size);

  public:
    As(string, string);

    string getPort() { return port; }
    int getName() { return name; }

    void bgp_send(char *port, unsigned char *msg, const int msg_len);
    void bgp_listen(char *port);

    map<string, string> setup_neighbours(string);
    void setup_listener();
    void setup_as(string);
    void neighbours_from_file(string);

    void keep_alive();
    void send_OPEN();

    unsigned char * generate_HEADER(unsigned char type, int *size, int msg_length);
    unsigned char * generate_OPEN(int *size);
    unsigned char * generate_UPDATE(int *size);
    unsigned char * generate_NOTIFICATION(int *size);
    unsigned char * generate_KEEPALIVE(int *size);

    //unsigned char * handle_msg(const unsigned char *msg, const int bytes_received);
    unsigned char * handle_msg(unsigned char const*, int);
};
