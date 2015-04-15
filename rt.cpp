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

// for inet_pton-related usage examples...
#include <sys/types.h>

//...
#include <arpa/inet.h>
#include <netinet/in.h>

int IPaddress2int(const char *IPaddress)
{
	int IPint = 0;
	inet_pton(AF_INET, IPaddress, &IPint ); // &(sa.sin_addr));
	return IPint;

	//printf("%s\n", str); // prints "192.0.2.33"
} // IPaddress2int

void printIPint(int IPint) // TO-DO! memory allocation in C sucks
{
	struct sockaddr_in sa;
	//char str[INET_ADDRSTRLEN]; http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#inet_ntopman
	char str[16];

	// now get it back and print it
	inet_ntop(AF_INET, &(sa.sin_addr), str, 16);

	printf("%s\n", str); // prints "192.0.2.33"
} // printIPint



class routeTable
{
// currently hard-coded limit for 1000 routes, I will implement dynamic allocation using e.g. <vector>

#define RMAX 1000

#ifndef VERBOSE
#define VERBOSE 3
#endif

struct routingItem_t
{
	unsigned int nextHop; // where to next (IP address of the neighbor AS)
	unsigned int target; // what is target IP address
	unsigned int mask; // mask generated with prefixlen
	unsigned int prefixlen; // IP/prefixlen, the effective bits in src
	string ASPATH; // for informative purposes or advanced routing decisions
	string ASNAME; // \to-do { should this be ASNAME as more descriptive convention }
	int priority; // pri 0 for default route, all other routest pri at least 1
	unsigned int trust;

};

struct routingTable_t
{
	routingItem_t ri[RMAX];
	int items; // how many table entries in table
} rt;

public:

routeTable() {

	// init rt item count:
	rt.items = 0;

}; // Constructor

~routeTable(){};// Destructor

int addRoute(string ASNAME, string ASPATH, unsigned int mask, unsigned int prefixlen, unsigned int nextHop, unsigned int target, int priority);
int addRoute(string ASNAME, string ASPATH, unsigned int mask, unsigned int prefixlen, unsigned int nextHop, unsigned int target, int priority,
			unsigned int trust);
int queryNextHop(string destinationASNAME);
string queryASPATH(int destinationIP);
int queryRoute(int destination);
int deleteRoute();
int routeCount();
};

/*int routeTable::routeTable()
{
	//return 0;
}*/

/*
 *
 */
int routeTable::addRoute(string ASNAME, string ASPATH,
							unsigned int mask,
							unsigned int prefixlen,
							unsigned int nextHop,
							unsigned int target,
							int priority)
{
	routingItem_t rtRow;

	rtRow.ASPATH = ASPATH;
	rtRow.mask = mask;
	rtRow.prefixlen = prefixlen;
	rtRow.nextHop = nextHop;
	rtRow.target = target;
	rtRow.priority = priority;
	rtRow.trust = 0; // default value for trust (NB! are the advantages if trust would be a signed int)

	rt.ri[rt.items] = rtRow;
	rt.items++;

	return rt.items;
} // ::addRoute (without trust)

/*
 * add route to routing table, with trust
 */
int routeTable::addRoute(string ASNAME, string ASPATH,
							unsigned int mask,
							unsigned int prefixlen,
							unsigned int nextHop,
							unsigned int target,
							int priority,
							unsigned int trust)
{
	routingItem_t rtRow;
	unsigned int newRouteIndex;

	newRouteIndex = addRoute(ASNAME, ASPATH, mask,
			prefixlen, nextHop, target, priority);

	rtRow = rt.ri[newRouteIndex];
	rtRow.trust = trust;

	return newRouteIndex;
} // ::addRoute (with trust)


/**
 *
 * @name    queryNextHop
 * @brief  	What is the next hop IP address for given ASNAME
 * @ingroup routeTable
 *
 * Finds the next hop for given ASNAME, even when the given ASNAME is not in the neighbours.
 * It must search thorough the router tables items ASNAMEs.
 *
 * @param [in] (string destinationASNAME) target ASNAME as string
 *
 * @retval int Returns IP address (IPv4 address) in int format
 *
 * TO-DO: actual implementation - now only "dummy" implementation, now gives the first item in routing table for testing purposes
 */
int routeTable::queryNextHop(string destinationASNAME)
{
	return rt.ri[0].nextHop;
}

/**
 *
 * @name    queryASPATH
 * @brief  	What is the ASPATH for given neighbor IP address
 * @ingroup routeTable
 *
 * What is the ASPATH for given neighbor IP address
 *
 * @param [in] (int destinationIP) target router IP address in int format
 *
 * @retval string Returns ASPATH for given neighbour router, or NULL if there is a problem deciding the ASPATH
 *
 * TO-DO: actual implementation - now only "dummy" implementation, now gives the first item ASPATH in routing table for testing purposes
 */
string routeTable::queryASPATH(int destinationIP)
{
	if ( rt.items > 0)
		return rt.ri[0].ASPATH; // initially just sent any valid ASNAME from the RT
	else
	{
		return NULL;
	}
}


/**
 * returns the AS IP address to send the packet to, for the packet that is on route to destination
 * return -1 on error (his should be rare situation. There should always be a default route, unless you are on the very top.)
 */
int routeTable::queryRoute(int destination)
{
	// \to-do { can we be sure there is no thread conflict that changes e.g. rt.items }
	int ri;
	int resultDst = -1; // used for result destination IP
	int resultPri = -9999999; // used for deciding the result // TO-DO: better lowest possible
	// iterate rt
	//printf("route items: %i\n", rt.items);
	routingItem_t rtRow; // to be used in iterations and comparisons
	routingItem_t rtRowBest; // Best match saved here
	int match_found = 0; // indicates if match is already found or not \to-do { match_found could be avoided if rtRowBest is null until first match is found}++

	for (ri = 0; ri < rt.items; ri++)
	{
		rtRow = rt.ri[ri];
		/*
		if (VERBOSE>2) printf("V3: route item: %i, pri:%i: ", ri, rtRow.priority);
		if ( rtRow.priority == 1 )
		{
			if (VERBOSE>3) cout << "V3: " << ri << ": " << "pri1=|" << rtRow.ASNAME << "| ";
			resultDst = rtRow.nextHop;
		}
		*/

		// calculate if match found for destination in route table entries

		if ( ( rtRow.mask & rtRow.target ) == ( rtRow.mask & destination ) )
		{
			if (VERBOSE>2) printf("V3: matchA1! prefixlen:%i\n", rtRow.prefixlen);
			if ( resultPri < rtRow.priority )
			{
				if (VERBOSE>3) printf("V4: matchA1! pri old -> new: %i %i\n", resultPri, rtRow.priority);
				resultDst = rtRow.nextHop;
				resultPri = rtRow.priority;
			}
		}

		// alternative / better / wrong way to compute match?!:
		if ( ( rtRow.target >> rtRow.prefixlen ) == ( destination >> rtRow.prefixlen ) )
		{
			if (VERBOSE>2) printf("V3: matchA2! prefixlen:%i\n", rtRow.prefixlen);
			if ( resultPri < rtRow.priority )
			{
				if (VERBOSE>3) printf("V4: matchA2! pri old -> new: %i %i\n", resultPri, rtRow.priority);
				resultDst = rtRow.nextHop;
				resultPri = rtRow.priority;
			}
		}
		cout << endl;
	} // if

	// if some destination was determined by the routing table, return it:
	if ( resultDst > -1 )
		return resultDst;
	else
	{
		// if no match could be found, return something, if possible:
		if ( rt.items > 0)
			return rt.ri[0].nextHop; // in case of trouble, return some valid AS IP from the RT
		else
			return 0; // if all fails, send "0" -- TO-DO: some error code should be emitted
	}
} // ::queryRoute

/**
 * @name    routeCount
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

