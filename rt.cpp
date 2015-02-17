/**
 * @file   rt.cpp
 * @Author Jussi Heino (juhehe@utu.fi
 * @date   Feb 2015
 * @brief  Route Table class impements a Internet routing table data structure and methods
 *
 * You can add routes to routing table.
 * Assumes int is 32-bit (at least).
 * Uses int to store a IPv4 address.
 *
 * \to-do { Currently a hard-coded limit of 1000 route entries}
 */

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
// currently hard-coded limit for 1000 routes, I will implement dynamic allocation using e.g. <vector>
#define RMAX 1000

struct routingItem_t
{
	unsigned int nextHop; // where to next (IP address of the neighbor AS)
	unsigned int target; // what is target IP address
	unsigned int mask; // mask generated with prefixlen
	unsigned int prefixlen; // IP/prefixlen, the effective bits in src
	string ASPATH; // for informative purposes or advanced routing decisions
	string ASID; // \to-do { should this be ASNAME as more descriptive convention }
	int priority; // pri 0 for default route, all other routest pri at least 1

};

struct routingTable_t
{
	routingItem_t ri[RMAX];
	int items; // how many table entries in table
} rt;

public:

routeTable() {

	// routing table, fixed side max 1000 entries now

	// init rt item count:
	rt.items = 0;

}; // Constructor

~routeTable(){};// Destructor

int addRoute(string ASID, string ASPATH, unsigned int mask, unsigned int nextHop, unsigned int target, int priority);
int queryRoute(int destination);
int deleteRoute();
int routeCount();
};

/*int routeTable::routeTable()
{
	//return 0;
}*/

int routeTable::addRoute(string ASID, string ASPATH,
							unsigned int mask,
							unsigned int nextHop,
							unsigned int target,
							int priority)
{
	routingItem_t rtRow;

	rtRow.ASPATH = ASPATH;
	rtRow.mask = mask;
	rtRow.nextHop = nextHop;
	rtRow.target = target;

	rt.ri[rt.items] = rtRow;
	rt.items++;

	return 0;
}

int routeTable::queryRoute(int destination)
{
	return rt.ri[0].nextHop;
}

/**
 * @name    route count
 * @brief   How many routes in routing table
 * @ingroup routetable
 *
 * Finds the number of routes in routing table.
 *
 * @param [in] ()- No parameters
 *
 * @retval int Returns zero or larger integer as the number of routes in routing table.
 *
 * Example Usage:
 * @code
 * 		myRT routeTable;
 * 		cout << myRT.routeCount(); // should output zero, since no routes were added
 * @endcode
 */
int routeTable::routeCount()
{
	return rt.items;
} // ::routeCount

