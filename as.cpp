#include "as.h"
#include "stdio.h"
#include "SystemV2.cpp"

//#include "as_bgp_listen.cpp"
//#include "as_bgp_send.cpp"

using namespace std;

//void As::bgp_send(char *port, unsigned char *msg, const int msg_len);
//void As::bgp_listen(char *port);

As::As (string as_config, string neighbours_config, string rt_config) {
  cout << ">>> Setting up AS..." << endl;
  As::setup_as(as_config);

  cout << ">>> Setting up neighbours..." << endl;
  neighbours = setup_neighbours(neighbours_config);

  cout << ">>> Setting up routing table..." << endl;
  As::rt_from_file(rt_config);
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
	  string client_port;
	  std::getline(lineStream, as_name, ',');
	  std::getline(lineStream, as_port, ',');
	  std::getline(lineStream, client_port, ',');
    //As::name = atoi(as_name.c_str());
    As::name = std::stoi(as_name);
    cout << "NAME: " << As::name << endl;
    As::port = as_port;
    cout << "PORT: " << As::port << endl;
    As::client_port = client_port;
    cout << "CLIENT PORT: " << As::client_port << endl;
	  config_file.close();
  }
}

map<int, string> As::setup_neighbours(string neighbours_config)
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
	    string tmp_name;
	    string nb_port;
	    string client_port;
	    std::getline(lineStream, tmp_name, ',');
	    std::getline(lineStream, nb_port, ',');
	    std::getline(lineStream, client_port, ',');

      //int nb_name = atoi( tmp_name.c_str() ); // stoi was incompatible with a Cygwin setup
      int nb_name = std::stoi(tmp_name);
	    neighbours[nb_name] = nb_port;
	    neighbours_client[nb_name] = client_port;
	    //neighbours_state[nb_name] = 0;
	    neighbours_state[nb_name] = 0;
	  }
	  config_file.close();
  }
  return neighbours;
  /* ========== END ========== */

}

void As::save_rt(string rt_config) {
  //cout << "============SAVING RT TO FILE===========" << endl;
  ofstream config_file;
  config_file.open (rt_config);
  for (int i = 0; i < As::rt.getSize(); i++) {
    RoutingItem item = As::rt.getItem(i);
    //cout << item.toString() << endl;
    config_file << item.toString() << "\n";
  }
  config_file.close();
}

void As::rt_from_file(string rt_config_filename) {
  string line;
  ifstream config_file;
  config_file.open( rt_config_filename );
  if ( !config_file.is_open() ) {
	  fprintf(stderr, "### error in opening file: ");
	  fprintf(stderr, "%s", rt_config_filename.c_str());
	  fprintf(stderr, "\n");
  }
  else {
	  while(getline(config_file, line))
	  {
	    stringstream lineStream(line);
      string destination;
      string next_hop;
      string path_length; 
      string priority;
      string path; //Format: "1 2 3 4"
	    std::getline(lineStream, destination, ',');
	    std::getline(lineStream, next_hop, ',');
	    std::getline(lineStream, path_length, ',');
	    std::getline(lineStream, priority, ',');
	    std::getline(lineStream, path, ',');
      
      cout << destination << endl;
      cout << next_hop << endl;
      cout << path_length << endl; 
      cout << priority << endl;
      cout << path << endl; //Format: "1 2 3 4"

      RoutingItem item;
      item.destination = std::stoi(destination);
      item.next_hop = std::stoi(next_hop);
      item.path_length = std::stoi(path_length); 
      item.priority = std::stoi(priority);
      stringstream pathStream(path);
      string as_node;
      item.path = (unsigned char *)malloc(item.path_length);
      int i = 0;
      while (getline(pathStream, as_node, ' ')) {
        //cout << "NODE: " << as_node << endl;
        item.path[i] = std::stoi(as_node);
        i++;
      }
      As::rt.addItem(item);
	  }
    As::rt.print_table();
	  config_file.close();
  }
}

/**
 *  Setup AS server
 */
void As::setup_listener()
{
  cout << ">>> Setting up listener..." << endl;
  char listen_port[10];
  strcpy(listen_port, As::port.c_str());

  // simple consistency checks for port read from file
  if ( ( strlen(listen_port)<6 )
	&& ( atoi(listen_port) > 0 )
	&& ( atoi(listen_port) < 65536 ) ) {
	  cout << "Listening port: " << listen_port << endl;
	  bgp_listen(listen_port);
	}
	else {
	  cout << "ERROR: Listen port syntax error."<< endl;
	}
}

/**
 *  Setup AS server
 */
void As::setup_client_listener()
{
  cout << ">>> Setting up client listener..." << endl;
  char listen_port[10];
  strcpy(listen_port, As::client_port.c_str());

  // simple consistency checks for port read from file
  if ( ( strlen(listen_port)<6 )
	&& ( atoi(listen_port) > 0 )
	&& ( atoi(listen_port) < 65536 ) ) {
	  client_listen(listen_port);
	  cout << "Client listening port: " << listen_port << endl;
	}
	else {
	  cout << "ERROR: Listen port syntax error."<< endl;
	}
}

void As::keep_alive() 
{
  while(true) {
    for (map<int, int>::iterator it=neighbours_state.begin(); it!=neighbours_state.end(); ++it) {
      if (it->second == NB_ON) {
        int as_name = it->first;

        if (neighbours.find(as_name) != neighbours.end()) {
          // Convert string to char[]
          char port[10];
          strcpy(port, neighbours[as_name].c_str());

          int size = 0;
          unsigned char *msg = As::generate_KEEPALIVE(&size);
          int status = bgp_send(port, msg, size);
          free(msg);
          cout << "=======================" << endl;
          cout << "KEEPALIVE SENT TO: " << port << endl;

          if (status == -1) {
            As::switch_neighbour(as_name, NB_OFF);
            As::withdrawn_nb_from_rt(as_name);
          } 
        }
      }
    } 
    sleep(3);
  }
}

void As::send_OPEN() 
{
  while (true) {
    for (map<int, int>::iterator it=neighbours_state.begin(); it!=neighbours_state.end(); ++it) {
      if (it->second == NB_OFF) {
        int as_name = it->first;

        if (neighbours.find(as_name) != neighbours.end()) {
          char port[10];
          strcpy(port, neighbours[as_name].c_str());

          int size = 0;
          unsigned char *msg = As::generate_OPEN(&size);
          int status = bgp_send(port, msg, size);
          free(msg);
          cout << "=======================" << endl;
          cout << "OPEN SENT TO: " << port << endl;
        }
      }
    } 
    sleep(3);
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
  header new_header;
  std::fill_n(new_header.marker, 16, '1');
  new_header.length += msg_length;
  new_header.type = type;

  unsigned char *buffer = (unsigned char *) malloc(HEADER_LENGTH * sizeof(unsigned char));
  unsigned char *ptr;
  ptr = serialize_HEADER( buffer, &new_header, size );

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

unsigned char * As::generate_OPEN(int *size) {
  open_msg new_open_msg;
  new_open_msg.my_as = As::name;

  int body_size = 0;
  unsigned char buffer[7], *msg_body;
  msg_body = serialize_OPEN( buffer, &new_open_msg, &body_size );

  //unsigned char header_buffer[HEADER_LENGTH], *msg_header;
  unsigned char *msg_header;
  msg_header = As::generate_HEADER(1, size, body_size);

  *size += body_size;
  unsigned char *total_msg = (unsigned char *)malloc(*size);
  memcpy(total_msg, msg_header, HEADER_LENGTH);
  memcpy(total_msg + HEADER_LENGTH, msg_body, body_size);

  free(msg_header);
  //free(msg_body);

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

unsigned char * As::generate_UPDATE(int *size, update_msg update_info) {
  int body_size = 0;
  unsigned char buffer[100], *msg_body;
  msg_body = serialize_UPDATE( buffer, &update_info, &body_size );

  unsigned char *msg_header;
  msg_header = As::generate_HEADER(2, size, body_size);

  *size += body_size;
  unsigned char *total_msg = (unsigned char *)malloc(*size);
  memcpy(total_msg, msg_header, HEADER_LENGTH);
  memcpy(total_msg + HEADER_LENGTH, msg_body, body_size);

  free(msg_header);

  return total_msg;
}

unsigned char * As::serialize_UPDATE(unsigned char * buffer, struct update_msg *value, int *size) {
  buffer = serialize_char(buffer, value->withdrawn_length, size);
  //cout << "UPDATE WITHDRAW_LENGTH:" << (int)value->withdrawn_length << endl;
  if ((int)value->withdrawn_length > 0) {
    for (int i = 0; i < (int)value->withdrawn_length; i++) {
      buffer = serialize_char(buffer, value->withdrawn_route[i], size);
      //cout << "UPDATE WITHDRAW_ROUTE:" << (int)value->withdrawn_route[i] << endl;
    }
  }
  buffer = serialize_char(buffer, value->path_length, size);
  //cout << "UPDATE PATH_LENGTH:" << (int)value->path_length << endl;
  if ((int)value->path_length > 0) {
    for (int i = 0; i < (int)value->path_length; i++) {
      buffer = serialize_char(buffer, value->path_value[i], size);
      //cout << "UPDATE PATH_VALUE:" << (int)value->path_value[i] << endl;
    }
  }
  return buffer - *size;
}

As::header As::deserialize_HEADER(unsigned char *header_stream) {
  header msg_header;
  for (int i = 0; i < 16; i++) {
    msg_header.marker[i] = header_stream[i];
  }
  msg_header.length = header_stream[16];
  msg_header.type = header_stream[17];
  return msg_header;
}

As::open_msg As::deserialize_OPEN(unsigned char *open_stream) {
  open_msg msg_open;
  msg_open.version = open_stream[0];
  msg_open.my_as = open_stream[1];
  msg_open.holdtime = open_stream[2];
  for (int i = 3; i < 7; i++) {
    msg_open.bgp_identifier[i - 3] = open_stream[i];
  }
  return msg_open;
}

As::update_msg As::deserialize_UPDATE(unsigned char *update_stream) {
  As::update_msg msg_update;
  if ((int)update_stream[0] > 0) {
    msg_update.withdrawn_length = update_stream[0];
    int length = (int)update_stream[0];
    msg_update.withdrawn_route = (unsigned char *) malloc(length);
    for (int i = 0; i < length; i++) {
      msg_update.withdrawn_route[i] = update_stream[i+1];
    }
  } else {
    msg_update.path_length = update_stream[1];
    int length = (int)update_stream[1];
    msg_update.path_value = (unsigned char *) malloc(length);
    for (int i = 0; i < length; i++) {
      msg_update.path_value[i] = update_stream[i+2];
    }
  }
  return msg_update;
}

unsigned char * As::handle_msg(const unsigned char *msg, const int bytes_received, int * size) {
  unsigned char *msg_return, status[1];
  if (bytes_received >= HEADER_LENGTH) {
    unsigned char *msg_header = (unsigned char *)malloc(HEADER_LENGTH);
    memcpy(msg_header, msg, HEADER_LENGTH);
    header header_str = deserialize_HEADER(msg_header);

    unsigned char *msg_body = (unsigned char *)malloc(header_str.length - HEADER_LENGTH);
    memcpy(msg_body, msg + HEADER_LENGTH, header_str.length - HEADER_LENGTH);

    // Handle OPEN msg
    if (header_str.type == OPEN_TYPE) {
      open_msg open_str = deserialize_OPEN(msg_body);
      //string as_name = to_string((int)open_str.my_as);
      int as_name = (int)open_str.my_as;
      As::switch_neighbour(as_name, NB_ON);
      As::add_nb_to_rt(as_name);

      // advertise routes
      thread advertise_thread(&As::advertise_routes, this, as_name);
      advertise_thread.detach();

      // return message
      msg_return = As::generate_OPEN(size);
    }
    // Handle UPDATE msg
    if (header_str.type == UPDATE_TYPE) {
      update_msg msg_update = deserialize_UPDATE(msg_body);
      if ((int)msg_update.withdrawn_length > 0) {
        As::remove_route(msg_update);
        // notify the network
        thread transfer_thread(&As::notify_removing, this, msg_update);
        transfer_thread.detach();
      }
      if ((int)msg_update.path_length > 0) {
        As::add_route(msg_update, 0);
        // notify the network
        thread transfer_thread(&As::notify_adding, this, msg_update);
        transfer_thread.detach();
      }

      *size = 1;
      status[0] = 0;
      msg_return = status;
    }

    free(msg_body);
  } else {
    *size = 1;
    status[0] = -1;
    msg_return = status;
  }

  return msg_return;
}

unsigned char * As::client_handle_msg(unsigned char *msg, const int bytes_received, int * size) {
  unsigned char *msg_return, status[1];
  if (bytes_received > 0) {
    cout << "===========CLIENT MSG RECEIVED============" << endl;
    Packet p;
	  p.deserialize(msg, PACKET_MAX_LEN);
    p.Print();
  }
  *size = 1;
  status[0] = 0;
  msg_return = status;
  return msg_return;
}

void As::switch_neighbour(int as_name, int state) {
  if (neighbours_state.find(as_name) != neighbours_state.end()) {
    neighbours_state[as_name] = state;
    cout << "=======================" << endl;
    cout << "NEIGHBOUR STATE UPDATED: " << as_name << " - " << neighbours_state[as_name] << endl;
  }
}

void As::add_route(update_msg msg_update, int priority = 0){
  int path_length = (int)msg_update.path_length;
  int destination = (int)msg_update.path_value[0]; // The first node is the target AS
  if (destination != As::name) {
    As::rt.addRoute(destination, path_length, msg_update.path_value, priority);

    As::rt.print_table();

    thread saving_thread(&As::save_rt, this, ROUTING_TABLE_FILENAME);
    saving_thread.detach();
  }
}

void As::remove_route(update_msg msg_update) {
  int withdrawn_length = (int)msg_update.withdrawn_length;
  int destination = (int)msg_update.withdrawn_route[0]; // The first node is the target AS
  if (destination != As::name) {
    //As::rt.removeRoute(destination, withdrawn_length, msg_update.withdrawn_route);
    As::rt.removeRoute(destination);

    As::rt.print_table();

    thread saving_thread(&As::save_rt, this, ROUTING_TABLE_FILENAME);
    saving_thread.detach();
  }
}

void As::notify_adding(update_msg msg_update) {
  int old_length = (int)msg_update.path_length;
  int destination = (int)msg_update.path_value[0];
  int sender = (int)msg_update.path_value[old_length - 1];
  unsigned char * new_path = (unsigned char *) malloc(old_length + 1);
  memcpy(new_path, msg_update.path_value, old_length);
  new_path[old_length] = As::name; // Add current AS to the end of the path
  msg_update.path_value = new_path;
  msg_update.path_length += 1;
  int msg_size = 0;
  unsigned char *msg= As::generate_UPDATE(&msg_size, msg_update);


  for (map<int, int>::iterator it=neighbours_state.begin(); it!=neighbours_state.end(); ++it) {
    if (it->second == NB_ON) {
      //int as_name = stoi(it->first);
      int as_name = it->first;
      if (as_name != destination && as_name != sender) {
        if (neighbours.find(as_name) != neighbours.end()) {
          char port[10];
          strcpy(port, neighbours[it->first].c_str());
          int status = bgp_send(port, msg, msg_size);
          cout << "=======================" << endl;
          cout << "UPDATE TRANSFERED TO: " << as_name << ":" << port << endl;
        }
      }
    }
  } 
  free(new_path);
  free(msg);
}

void As::notify_removing(update_msg msg_update) {
  int old_length = (int)msg_update.withdrawn_length;
  int destination = (int)msg_update.withdrawn_route[0];
  int sender = (int)msg_update.withdrawn_route[old_length - 1];
  unsigned char * new_path = (unsigned char *) malloc(old_length + 1);
  memcpy(new_path, msg_update.withdrawn_route, old_length);
  new_path[old_length] = As::name; // Add current AS to the end of the path
  msg_update.withdrawn_route = new_path;
  msg_update.withdrawn_length += 1;
  int msg_size = 0;
  unsigned char *msg= As::generate_UPDATE(&msg_size, msg_update);


  for (map<int, int>::iterator it=neighbours_state.begin(); it!=neighbours_state.end(); ++it) {
    if (it->second == NB_ON) {
      //int as_name = stoi(it->first);
      int as_name = it->first;
      if (as_name != destination && as_name != sender) {
        if (neighbours.find(as_name) != neighbours.end()) {
          char port[10];
          strcpy(port, neighbours[it->first].c_str());
          int status = bgp_send(port, msg, msg_size);
          cout << "=======================" << endl;
          cout << "UPDATE TRANSFERED TO: " << as_name << ":" << port << endl;
        }
      }
    }
  } 
  free(new_path);
  free(msg);
}

void As::add_nb_to_rt(int as_name){
  update_msg msg_update; 
  msg_update.path_length = 1;
  msg_update.path_value = (unsigned char *) malloc(1);
  msg_update.path_value[0] = as_name;
  if (As::rt.findRoute(as_name, 1, msg_update.path_value) == NULL) {
    cout << "===========ADDING CALLED============" << endl;
    As::add_route(msg_update, 0);

    // notify the network
    thread transfer_thread(&As::notify_adding, this, msg_update);
    transfer_thread.detach();
  }
}

void As::withdrawn_nb_from_rt(int as_name) {
  update_msg msg_update; 
  msg_update.withdrawn_length = 1;
  msg_update.withdrawn_route = (unsigned char *) malloc(1);
  msg_update.withdrawn_route[0] = as_name;
  if (As::rt.findRoute(as_name, 1, msg_update.withdrawn_route) != NULL) {
    cout << "===========WITHDRAWN CALLED============" << endl;
    As::remove_route(msg_update);

    // notify the network
    thread transfer_thread(&As::notify_removing, this, msg_update);
    transfer_thread.detach();
  }
}

void As::advertise_routes(int receiver) {
  if (neighbours.find(receiver) != neighbours.end()) {
    for (int i = 0; i < As::rt.getSize(); i++) {
      RoutingItem item = As::rt.getItem(i);
      int destination = (int)item.path[0];
      int sender = (int)item.path[item.path_length - 1];

      if (receiver != destination && receiver != sender) {
        update_msg msg_update; 
        msg_update.path_length = item.path_length + 1;
        msg_update.path_value = (unsigned char *) malloc(msg_update.path_length);
        memcpy(msg_update.path_value, item.path, item.path_length);
        msg_update.path_value[item.path_length] = As::name;

        int msg_size = 0;
        unsigned char *msg= As::generate_UPDATE(&msg_size, msg_update);

        free(msg_update.path_value);

        char port[10];
        strcpy(port, neighbours[receiver].c_str());
        int status = bgp_send(port, msg, msg_size);
        cout << "=======================" << endl;
        cout << "ADVERTISING " << item.destination << " TO " << receiver << endl;

        free(msg);
      }
    }
  }
}

void As::self_advertise() {
  while (true) {
    for (map<int, int>::iterator it=neighbours_state.begin(); it!=neighbours_state.end(); ++it) {
      if (it->second == NB_ON) {
        update_msg new_update_msg; 
        new_update_msg.path_length = 1;
        new_update_msg.path_value = (unsigned char *) malloc(1);
        new_update_msg.path_value[0] = As::name;
        //cout << "UPDATE INITIAL PATH_VALUE: " << (int)new_update_msg.path_value[0] << endl;
        
        int msg_size = 0;
        unsigned char *msg= As::generate_UPDATE(&msg_size, new_update_msg);

        free(new_update_msg.path_value);

        int as_name = it->first;
        if (neighbours.find(as_name) != neighbours.end()) {
          char port[10];
          strcpy(port, neighbours[as_name].c_str());
          int status = bgp_send(port, msg, msg_size);
          cout << "=======================" << endl;
          cout << "UPDATE SENT TO: " << port << endl;
        }

        free(msg);
      }
    } 
    sleep(3);
  }
}

void As::client_communication_simulation() {
  if (As::name == 1) {
    while (true) {
      int destination = 3; // for testing 
      RoutingItem item = rt.queryRoute(destination);
      if ( item.destination != 0 ) {
        cout << "============CLIENT COMMUNICATION===========" << endl;
        item.print();

	      Packet p = Packet(4, 0, 0, 0, 0, 255, 6, As::name, destination);
        int next_hop = item.next_hop;

        char port[10];
        strcpy(port, neighbours_client[next_hop].c_str());
        cout << "PORT: " << port << endl;

        int size = 0;
	      unsigned char * msg = p.serialize();
        //unsigned char *msg = As::generate_HEADER(4, &size);

        int status = client_send(port, msg, 20);
      }
      sleep(3);
    }
  }
}

void As::run() {
  thread listener_thread(&As::setup_listener, this);
  listener_thread.detach();

  thread client_listener_thread(&As::setup_client_listener, this);
  client_listener_thread.detach();

  //thread KEEPALIVE_thread(&As::keep_alive, this);
  //KEEPALIVE_thread.detach();

  thread open_thread(&As::send_OPEN, this);
  open_thread.detach();

  // Send UPDATE msg to self advertise
  //thread advertise_thread(&As::self_advertise, this);
  //advertise_thread.detach();

  thread client_thread(&As::client_communication_simulation, this);
  client_thread.detach();

  As::keep_alive();
}

int main()
{
  As as ( SETUP_CONFIG_FILENAME, SETUP_NEIGHBOUR_FILENAME, 
    ROUTING_TABLE_FILENAME);
  as.run();
  return 0;
}
