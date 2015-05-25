#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>
#include <thread>
#include <fstream>
#include <string> // for std::string, std::to_string
#include <sstream> // for std::stringstream
#include <map> // for std::map
#include <stdlib.h> // for std::sleep

using namespace std;

class RoutingItem {
  public:
    int destination; // Instead of using IP prefixes, we use AS name to indicate AS
    int next_hop; // next router to send the packet to
    int path_length; 
    unsigned char * path; //Format: "1 2 3 4"
    int priority;

    int getNextHop() { return next_hop; }
    void print() { 
      cout << "============ROUTING ITEM============" << endl;
      cout << "Name: " << destination << endl;
      cout << "Next Hop: " << next_hop << endl;
      cout << "Path Length: " << path_length << endl;
      cout << "Priority: " << priority << endl;
      cout << "Path: " ;
      for (int i=0; i < path_length; i++) {
        cout << (int)path[i] << " ";
      }
      cout << endl;
    }
    string toString() {
      string data = "";
      char tmps [200];// for temp storing a string, to avoid using C++11 std::to_string

      snprintf(tmps,199,"%i,%i,%i,%i", destination, next_hop, path_length, priority);
      //data += to_string(destination) + ',' +
      //  to_string(next_hop) + ',' + to_string(path_length) +
      //  ',' + to_string(priority) + ',';
      data = tmps;

      int tmpi;
      string tempString;
      for (int i=0; i < path_length; i++) {

    	//data += std::to_string((int)path[i]);
        tmpi = (int)path[i];
        snprintf(tmps, 199, "%i", tmpi);
        tempString = "";
        tempString = tmps;
        data += tempString;

        if ( i < (path_length - 1)) data += " ";
      }
      return data;
    }
};

#define RT_SIZE 1000
class RoutingTable {
  int size = 0;
  RoutingItem items[RT_SIZE];

  public:
    int getSize() { return RoutingTable::size; }
    void addItem(RoutingItem item);
    RoutingItem * getItems() { return RoutingTable::items; }
    RoutingItem getItem(int index) { return RoutingTable::items[index]; }
    void addRoute(int as_name, int path_length, unsigned char * path, int priority);
    //int removeRoute(int destination, int path_length, unsigned char * path);
    int removeRoute(int destination);
    int containNode(RoutingItem item, int as_name);
    void setRoutePriority(int as_name, unsigned char * path, int priority);
    RoutingItem queryRoute(int destination);
    RoutingItem * findRoute(int as_name, int path_length, unsigned char * path);
    //int indexOfRoute(int as_name, int path_length, unsigned char * path);
    void print_table();
};

#define NB_OFF 0
#define NB_ON 1
#define SETUP_CONFIG_FILENAME "as_configs"
#define SETUP_NEIGHBOUR_FILENAME "neighbours.csv"
#define ROUTING_TABLE_FILENAME "routing_table.csv"
#define PACKET_LENGTH 1500
#define KEEPALIVE_INTERVAL 5
#define OPEN_INTERVAL 5
#define CLIENT_MSG_INTERVAL 5
class As {
  string port;
  string client_port;
  int name;
  //map<string, string> neighbours;
  //map<string, int> neighbours_state;
  map<int, string> neighbours;
  map<int, int> neighbours_state;
  map<int, string> neighbours_client;
  RoutingTable rt;
  const int HEADER_LENGTH = 18;
  const int OPEN_TYPE = 1;
  const int UPDATE_TYPE = 2;
  const int NOTIFICATION_TYPE = 3;
  const int KEEPALIVE_TYPE = 4;

  //unsigned char * serialize_int(unsigned char *buffer, int value);
  unsigned char * serialize_char(unsigned char *buffer, char value, int *size);

  // Total: 18 octets
  struct header {
    unsigned char marker [16];
    unsigned char length = 18; //This should be 2 octets. Let's assume it 1octet atm.
    unsigned char type;
  };
  unsigned char * serialize_HEADER(unsigned char * buffer, struct header *value, int *size);

  // Total: 7 octets
  struct open_msg {
    unsigned char version = 4;
    unsigned char my_as; //should be 2 octets
    unsigned char holdtime = 3; //Should be 2 octets
    unsigned char bgp_identifier [4] = {192, 168, 0 ,1}; // Hardcode AS IP
    //Atm, let's ignore optional parameters
  };
  unsigned char * serialize_OPEN(unsigned char * buffer, struct open_msg *value, int *size);

  //ASSUMPTION: this is a simple custom update msg.
  struct update_msg {
    unsigned char withdrawn_length = 0;
    unsigned char * withdrawn_route;
    unsigned char path_length = 0;
    unsigned char * path_value;
  };
  unsigned char * serialize_UPDATE(unsigned char * buffer, struct update_msg *value, int *size);

  public:
    As(string, string, string);

    string getPort() { return port; }
    int getName() { return name; }

    int bgp_send(char *port, unsigned char *msg, const int msg_len);
    void bgp_listen(char *port);
    int client_send(char *port, unsigned char *msg, const int msg_len);
    void client_listen(char *port);

    map<int, string> setup_neighbours(string);
    void setup_listener();
    void setup_client_listener();
    void setup_as(string);
    void neighbours_from_file(string);
    void save_rt(string);
    void rt_from_file(string);

    void keep_alive();
    void send_OPEN();
    void ack_OPEN();

    unsigned char * generate_HEADER(unsigned char type, int *size, int msg_length);
    unsigned char * generate_OPEN(int *size);
    unsigned char * generate_UPDATE(int *size, update_msg msg);
    unsigned char * generate_NOTIFICATION(int *size);
    unsigned char * generate_KEEPALIVE(int *size);

    header deserialize_HEADER(unsigned char *);
    open_msg deserialize_OPEN(unsigned char *);
    update_msg deserialize_UPDATE(unsigned char *);

    void switch_neighbour(int as_name, int state); //state = 0 (off) || 1 (on)
    void add_route(update_msg, int priority);
    void remove_route(update_msg);
    unsigned char * handle_msg(unsigned char const* msg, int bytes_received, int * byte_sending);
    unsigned char * client_handle_msg(unsigned char * msg, int bytes_received, int * byte_sending);

    void withdrawn_nb_from_rt(int as_name);
    void add_nb_to_rt(int as_name);
    void notify_adding(update_msg);
    void notify_removing(update_msg);
    void advertise_routes(int as_name); // send rt items to an AS
    // Simulate clients' IP packet communication
    void client_communication_simulation(); 
    void run();
};
