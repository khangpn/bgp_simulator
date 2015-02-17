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

class routeTable
{
#define RMAX 1000
	struct routingItem_t
		{
			unsigned long int nextHop; // where to next
			unsigned long int target; // what is target IP address
			unsigned long int mask; // mask generated with prefixlen
			unsigned long int prefixlen; // IP/prefilen, the effectice bits in src
			string ASPATH; // for informative purposes or advanced routing decisions
		};
	struct routingTable_t
	{
		routingItem_t ri[RMAX];
		int items; // how many table entries in table
	} rt;

public:



routeTable() {

	routingItem_t rtRow;

	// routing table, fixed side max 1000 entries now

	// init rt:
	rt.items = 0;

	// method: add items to rt
	string s = "50 701 42";
	rtRow.ASPATH = s;
	rtRow.mask = 24;
	rtRow.nextHop = 10*8*8*8+ 10*8*8+  0*8+1;
	rtRow.target = 130*8*8*8+232*8*8+232*8+0;
	rt.ri[rt.items] = rtRow;
	rt.items++;


}; // Constructor

~routeTable(){};// Destructor
//int addRoute();
int addRoute(string ASPATH, unsigned long int mask, unsigned long int nextHop, unsigned long int target);
int queryRoute(int destination);
int deleteRoute();
int RouteCount();
};

/*int routeTable::routeTable()
{
	//return 0;
}*/

int routeTable::addRoute(string ASPATH, unsigned long int mask, unsigned long int nextHop, unsigned long int target)
{
	routingItem_t rtRow;
	rtRow.ASPATH = ASPATH;
	rtRow.mask = 24;
	rtRow.nextHop = 10*8*8*8+ 10*8*8+  0*8+1;
	rtRow.target = 130*8*8*8+232*8*8+232*8+0;
	rt.ri[rt.items] = rtRow;
	rt.items++;


	return 0;
}

int routeTable::queryRoute(int destination)
{
	return 1;
}

int routeTable::RouteCount()
{
	return rt.items;
}

