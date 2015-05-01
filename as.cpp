#include "as.h"
//#include "as_bgp_listen.cpp"
//#include "as_bgp_send.cpp"

#define SETUP_CONFIG_FILENAME "as_configs"
#define SETUP_NEIGHBOUR_FILENAME "neighbours.csv"
#define PACKET_LENGTH 1500

using namespace std;

//void As::bgp_send(char *port, unsigned char *msg, const int msg_len);
//void As::bgp_listen(char *port);

As::As (string as_config, string neighbours_config) {
  cout << ">>> Setting up AS..." << endl;
  As::setup_as(as_config);

  cout << ">>> Setting up neighbours..." << endl;
  neighbours = setup_neighbours(neighbours_config);

}

void As::neighbours_from_file(string config) {
  cout << ">>> Setting up neighbours..." << endl;
  As::neighbours = As::setup_neighbours(config);
}

void As::setup_as(string as_config) {
  ifstream config_file;
  config_file.open( as_config );
  if ( !config_file.is_open() ) {
	  fprintf(stderr, "### error in opening file: ");
	  fprintf(stderr, "%s", as_config.c_str());
	  fprintf(stderr, "\n");
  }
  else {
    string line;
	  getline(config_file, line);
	  stringstream lineStream(line);
	  string as_name;
	  string as_port;
	  std::getline(lineStream, as_name, ',');
	  std::getline(lineStream, as_port, ',');
    cout << "NAME: " << as_name << endl;
    As::name = std::stoi(as_name);
    cout << "PORT: " << as_port << endl;
    As::port = as_port;
	  config_file.close();
  }
}

map<string, string> As::setup_neighbours(string neighbours_config)
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
  //string port;
  //ifstream config_file;
  //config_file.open( as_config );
  //if ( config_file.is_open() ) {
	//  getline(config_file, port);
	//  config_file.close();

	//  // Convert string to char[]
	//  char listen_port[10];
	//  strcpy(listen_port, port.c_str());

	//  // simple consistency checks for port read from file
	//  if ( strlen(listen_port)<6 )
	//	  if ( atoi(listen_port) > 0 )
	//		  if ( atoi(listen_port) < 65536 )
	//			  bgp_listen(listen_port);
	//  	  	  // to-do: else

	//  config_file.close();
  //}
  //else { // config file cannot be opened
	//  fprintf(stderr, "### Config file %s open error", as_config.c_str());
  //}
    /* ========== END ========== */

  cout << ">>> Setting up listener..." << endl;
	// Convert string to char[]
	char listen_port[10];
	strcpy(listen_port, port.c_str());
  cout << "Listening port: " << listen_port << endl;

	// simple consistency checks for port read from file
	if ( strlen(listen_port)<6 )
		if ( atoi(listen_port) > 0 )
			if ( atoi(listen_port) < 65536 )
				bgp_listen(listen_port);
}

void As::keep_alive() 
{
  while(true) {
    for (map<string, string>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it) {
      //cout << it->first << " " << it->second << endl;
      // Convert string to char[]
      char port[10];
      strcpy(port, it->second.c_str());

      int size = 0;
      unsigned char *msg = As::generate_KEEPALIVE(&size);
      bgp_send(port, msg, size);
      free(msg);
    } 
    //cout << ">>> KEEPALIVE sent" << endl;
    sleep(3);
  }
}

void As::send_OPEN() 
{
  for (map<string, string>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it) {
    cout << ">>> Sending OPEN to: " << port << endl;
    char port[10];
    strcpy(port, it->second.c_str());

    int size = 0;
    unsigned char *msg = As::generate_OPEN(&size);
    bgp_send(port, msg, size);
    free(msg);
    cout << ">>> OPEN sent to: " << port << endl;
  } 
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

unsigned char * As::generate_HEADER(unsigned char type, int *size, int msg_length = 0) {
  std::fill_n(header.marker, 16, '1');
  header.length += msg_length;
  header.type = type;

  unsigned char *buffer = (unsigned char *) malloc(HEADER_LENGTH * sizeof(unsigned char));
  unsigned char *ptr;
  ptr = serialize_HEADER( buffer, &header, size );

  return ptr;
}

unsigned char * As::serialize_HEADER(unsigned char *buffer, struct header *value, int *size)
{
  for (int i = 0; i <= sizeof(value->marker) - 1; i++) {
    buffer = serialize_char(buffer, value->marker[i], size);
  }
  buffer = serialize_char(buffer, value->length, size);
  buffer = serialize_char(buffer, value->type, size);
  return buffer - *size;
}

unsigned char * As::generate_KEEPALIVE(int *size) {
  unsigned char *msg = As::generate_HEADER(4, size);
  return msg;
} 

    //unsigned char version = 4;
    //unsigned char my_as; //should be 2 octets
    //unsigned char holdtime = 3; //Should be 2 octets
    //unsigned char bgp_identifier [4] = {192, 168, 0 ,1}; // Hardcode AS IP
unsigned char * As::generate_OPEN(int *size) {
  open_msg.my_as = As::name;

  int body_size = 0;
  unsigned char buffer[7], *msg_body;
  msg_body = serialize_OPEN( buffer, &open_msg, &body_size );

  unsigned char header_buffer[HEADER_LENGTH], *msg_header;
  msg_header = As::generate_HEADER(1, size);

  *size += body_size;
  unsigned char *total_msg = (unsigned char *)malloc(*size);
  memcpy(total_msg, msg_header, HEADER_LENGTH);
  memcpy(total_msg + HEADER_LENGTH, msg_body, body_size);

  free(msg_header);

  return total_msg;
}

unsigned char * As::serialize_OPEN(unsigned char * buffer, struct open_msg *value, int *size) {
  buffer = serialize_char(buffer, value->version, size);
  buffer = serialize_char(buffer, value->my_as, size);
  buffer = serialize_char(buffer, value->holdtime, size);
  for (int i = 0; i <= sizeof(value->bgp_identifier) - 1; i++) {
    buffer = serialize_char(buffer, value->bgp_identifier[i], size);
  }
  //cout << "msg: " << (int)(*(buffer-7)) << endl;
  return buffer - *size;
}

unsigned char * As::handle_msg(const unsigned char *msg, const int bytes_received) {
  for (int i = 0; i <  bytes_received; i++) {
    if (i >= 16) {
      cout << (int)msg[i] << endl;
    } else {
      cout << msg[i] << endl;
    }
  }
  unsigned char *msg_return, status[1];
  status[0] = 0;
  msg_return = status;
  return msg_return;
}

int main()
{
  As as ( SETUP_CONFIG_FILENAME, SETUP_NEIGHBOUR_FILENAME );

  thread thread1(&As::setup_listener, as);
  thread1.detach();

  //as.keep_alive();
  as.send_OPEN();
  return 0;
}
