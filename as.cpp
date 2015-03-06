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

void bgp_send(char *port, char *message);
void bgp_listen(char *port);
void setup_listener();
map<string, string> setup_neighbours();

int main()
{
  map<string, string> neighbours;

  cout << ">>> Setting up AS..." << endl;
  thread thread1(setup_listener);
  thread1.detach();

  cout << ">>> Setting up neighbours..." << endl;
  neighbours = setup_neighbours();

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
//sleep(5);
cout << endl;
sleep(15);
  } // while
  // end of SAMPLE

  return 0;
}

map<string, string> setup_neighbours()
{
  map<string, string> neighbours;
  /* Setup AS links */
  // Read configuration file
  string line;
  ifstream links_file;
  ifstream config_file;
  config_file.open("neighbours.csv");
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
  return neighbours;
  /* ========== END ========== */

}

void setup_listener()
{
  /* Setup AS server */
  // Read configuration file
  string port;
  ifstream config_file;
  config_file.open("as_configs");
  getline(config_file, port);
  config_file.close();

  // Convert string to char[]
  char listen_port[10];
  strcpy(listen_port, port.c_str());

  bgp_listen(listen_port);

  config_file.close();
  /* ========== END ========== */
}
