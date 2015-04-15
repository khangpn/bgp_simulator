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

#define SETUP_CONFIG_FILENAME "as_configs"
#define SETUP_NEIGHBOUR_FILENAME "neighbours.csv"


using namespace std;

void bgp_send(char *port, char *message);
void bgp_listen(char *port);

class As {
  int port;
  string name;
  map<string, string> neighbours;
  string as_config, neighbours_config;
  public:
    As(int, string);
    As(string, string);
    map<string, string> setup_neighbours();
    void setup_listener();
    void set_as_config(string);
    void set_neighbours_config(string);
    void run();
    int getPort() { return port; }
    string getName() { return name; }
};

As::As (int p, string n) {
  port = p;
  name = n;
}

As::As (string as_config, string neighbours_config) {
  As::as_config = as_config;
  As::neighbours_config = neighbours_config;
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
  ifstream links_file;
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

void As::run() 
{
  cout << ">>> Setting up AS..." << endl;
  thread thread1(&As::setup_listener, this);
  thread1.detach();

  cout << ">>> Setting up neighbours..." << endl;
  neighbours = setup_neighbours(); // NOTE: this should be put in a thread later

  // SAMPLE: Send a message to all neighbour every 5s
  while(true) {
    for (map<string, string>::iterator it=neighbours.begin(); it!=neighbours.end(); ++it) {
      //cout << it->first << " " << it->second << endl;
      // Convert string to char[]
      char port[10];
      strcpy(port, it->second.c_str());

      char msg[] = "Hello ";
      strcat(msg, port);
      bgp_send(port, msg);
    } // for
    cout << ">>> Message sent" << endl;
    cout << endl;
    sleep(15);
  } // while
  // end of SAMPLE
}

int main()
{
  As as ( SETUP_CONFIG_FILENAME, SETUP_NEIGHBOUR_FILENAME );
  as.run();
  return 0;
}
