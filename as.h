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

class RoutingItem {
  public:
    int as_name; // Instead of using IP prefixes, we use AS name to indicate AS
    int path_length; 
    unsigned char * path; //Format: "1 2 3 4"
    int priority;
};

#define RT_SIZE 1000
class RoutingTable {
  int size = 0;
  RoutingItem items[RT_SIZE];

  public:
    int getSize() { return RoutingTable::size; }
    RoutingItem * getItems() { return RoutingTable::items; }
    RoutingItem getItem(int index) { return RoutingTable::items[index]; }
    void addRoute(int as_name, int path_length, unsigned char * path, int priority);
    void removeRoute(int as_name, int path_length, unsigned char * path);
    void setRoutePriority(int as_name, unsigned char * path, int priority);
    RoutingItem * getRoutes(int as_name);
    RoutingItem * getRouteByPath(int as_name, unsigned char * path);
    RoutingItem * findRoute(int as_name, int path_length, unsigned char * path);
    int indexOfRoute(int as_name, int path_length, unsigned char * path);
    void print_table();
};

#define NB_OFF 0
#define NB_ON 1
class As {
  string port;
  int name;
  //map<string, string> neighbours;
  //map<string, int> neighbours_state;
  map<int, string> neighbours;
  map<int, int> neighbours_state;
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
    As(string, string);

    string getPort() { return port; }
    int getName() { return name; }

    int bgp_send(char *port, unsigned char *msg, const int msg_len);
    void bgp_listen(char *port);

    map<int, string> setup_neighbours(string);
    void setup_listener();
    void setup_as(string);
    void neighbours_from_file(string);

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

    void withdrawn_nb_from_rt(int as_name);
    void add_nb_to_rt(int as_name);
    void self_advertise();
    void notify_adding(update_msg);
    void notify_removing(update_msg);
    void advertise_routes(int as_name); // send rt items to an AS
    void run();
};
