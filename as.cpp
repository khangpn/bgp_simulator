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
#include "as_bgp_listen.cpp"
#include "as_bgp_send.cpp"

#define SETUP_CONFIG_FILENAME "as_configs"
#define SETUP_NEIGHBOUR_FILENAME "neighbours.csv"
#define PACKET_LENGTH 1500

using namespace std;

void bgp_send(char *port, char *message);
void bgp_listen(char *port);

class As {
  int port;
  string name;
  map<string, string> neighbours;
  string as_config, neighbours_config;
  const int HEADER_LENGTH = 18;

  unsigned char * serialize_int(unsigned char *buffer, int value);
  unsigned char * serialize_char(unsigned char *buffer, char value, int *size);

  // Total: 18 octets
  struct header {
    unsigned char marker [16];
    unsigned char length = 19; //This should be 2 octets. Let's assume it 1octet atm.
    /*
      message types:
      1 - OPEN
      2 - UPDATE
      3 - NOTIFICATION
      4 - KEEPALIVE
    */
    unsigned char type;
  } header;
  unsigned char * serialize_header(unsigned char * buffer, struct header *value, int *size);

  struct open {
    unsigned char version [16];
    unsigned char my_as;
    unsigned char holdtime = 0; //Should be 2 octets
    unsigned char bgp_identifier [4];
    //Atm, let's ignore optional parameters
  } open;
  unsigned char * serialize_OPEN(unsigned char * buffer, struct open *value, int *size);

  public:
    As(string, string);
    map<string, string> setup_neighbours();
    void setup_listener();
    void set_as_config(string);
    void set_neighbours_config(string);
    void keep_alive();
    int getPort() { return port; }
    string getName() { return name; }
    unsigned char * generate_header(unsigned char type, int *size);
    unsigned char * generate_OPEN(int *size);
    unsigned char * generate_UPDATE(int *size);
    unsigned char * generate_NOTIFICATION(int *size);
    unsigned char * generate_KEEPALIVE(int *size);
};

As::As (string as_config, string neighbours_config) {
  As::as_config = as_config;
  As::neighbours_config = neighbours_config;

  cout << ">>> Setting up neighbours..." << endl;
  neighbours = setup_neighbours();

}

void As::set_as_config(string config) {
  As::neighbours_config = config;
}

void As::set_neighbours_config(string config) {
  As::as_config = config;
}

map<string, string> As::setup_neighbours()
{
  /* Setup AS links */
  // Read configuration file
  string line;
  ifstream config_file;
  config_file.open( neighbours_config );
  if ( !config_file.is_open() ) {
	  fprintf(stderr, "### error in opening file: ");
	  fprintf(stderr, "%s", neighbours_config.c_str());
	  fprintf(stderr, "\n");
  }
  else {
	  while(getline(config_file, line))
	  {
	    stringstream lineStream(line);
	    string nb_name;
	    string nb_port;
	    std::getline(lineStream, nb_name, ',');
	    std::getline(lineStream, nb_port, ',');
	    neighbours[nb_name] = nb_port;
	  }
	  config_file.close();
  }
  return neighbours;
  /* ========== END ========== */

}

void As::setup_listener()
{
  /* Setup AS server */
  // Read configuration file
  string port;
  ifstream config_file;
  config_file.open( as_config );
  if ( config_file.is_open() ) {
	  getline(config_file, port);
	  config_file.close();

	  // Convert string to char[]
	  char listen_port[10];
	  strcpy(listen_port, port.c_str());

	  // simple consistency checks for port read from file
	  if ( strlen(listen_port)<6 )
		  if ( atoi(listen_port) > 0 )
			  if ( atoi(listen_port) < 65536 )
				  bgp_listen(listen_port);
	  	  	  // to-do: else

	  config_file.close();
  }
  else { // config file cannot be opened
	  fprintf(stderr, "### Config file %s open error", as_config.c_str());
  }
    /* ========== END ========== */
}

void As::keep_alive() 
{
  //cout << ">>> Setting up AS..." << endl;
  //thread thread1(&As::setup_listener, this);
  //thread1.detach();

  // SAMPLE: Send a message to all neighbour every 5s
  while(true) {
    for (map<string, string>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it) {
      //cout << it->first << " " << it->second << endl;
      // Convert string to char[]
      char port[10];
      strcpy(port, it->second.c_str());

      int size = 0;
      unsigned char *msg = As::generate_KEEPALIVE(&size);

      // NOTE: if print out things here, the pointer will point to wrong mem
      //cout << "Msg to send: " << (int)msg[16] << endl;
      //for (int i = 0; i <=  17; i++) {
      //  if (i >= 16) {
      //    cout << (int)*(msg+i) << endl;
      //  } else {
      //    cout << *(msg+i) << endl;
      //  }
      //}
      bgp_send(port, msg, size);
    } 
    cout << ">>> Message sent" << endl;
    sleep(3);
  } // while
  // end of SAMPLE
}

unsigned char * As::generate_header(unsigned char type, int *size) {
  std::fill_n(header.marker, 16, '1');
  header.length = 18;
  header.type = type;

  unsigned char buffer[18], *ptr;
  ptr = serialize_header( buffer, &header, size );

  // Comparing ptr and buffer
  cout << ptr << endl;
  cout << buffer << endl;

  // Just print out what inside buffer
  //for (int i = 0; i <=  17; i++) {
  //  if (i >= 16) {
  //    cout << (int)ptr[i] << endl;
  //  } else {
  //    cout << ptr[i] << endl;
  //  }
  //}
  return ptr;
}

unsigned char * As::serialize_header(unsigned char *buffer, struct header *value, int *size)
{
  for (int i = 0; i <= sizeof(value->marker) - 1; i++) {
    buffer = serialize_char(buffer, value->marker[i], size);
  }
  buffer = serialize_char(buffer, value->length, size);
  buffer = serialize_char(buffer, value->type, size);
  cout << "Size: " << size << endl;
  return buffer - *size;
}

// NOTE: dont need this at the moment, we consider all components inside struct is char
//unsigned char * As::serialize_int(unsigned char *buffer, int value)
//{
//  /* Write big-endian int value into buffer; assumes 32-bit int and 8-bit char. */
//  buffer[0] = value >> 24;
//  buffer[1] = value >> 16;
//  buffer[2] = value >> 8;
//  buffer[3] = value;
//  return buffer + 4;
//}

unsigned char * As::serialize_char(unsigned char *buffer, char value, int *size)
{
  buffer[0] = value;
  *size += 1;
  return buffer + 1;
}

unsigned char * As::generate_KEEPALIVE(int *size) {
  unsigned char *msg = As::generate_header(4, size);
  // NOTE: if print out things here, the pointer will point to wrong mem
  //cout << "msg: " << (int)(*(msg+16)) << endl;
  //for (int i = 0; i <=  17; i++) {
  //  if (i >= 16) {
  //    cout << (int)msg[i] << endl;
  //  } else {
  //    cout << msg[i] << endl;
  //  }
  //}
  return msg;
} 

int main()
{
  As as ( SETUP_CONFIG_FILENAME, SETUP_NEIGHBOUR_FILENAME );

  cout << ">>> Setting up AS..." << endl;
  thread thread1(&As::setup_listener, as);
  thread1.detach();

  as.keep_alive();
  return 0;
}
