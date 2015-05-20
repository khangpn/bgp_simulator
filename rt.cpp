/**
 * @file   rt.cpp
 * @Author Jussi Heino (juhehe@utu.fi
 * @date   Feb-May 2015
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
#include <stdio.h>// for Jussi's sprintf temporary quick fix conversions

using namespace std;

// for inet_pton-related usage examples:
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

// define for Intel-style processors:
#define HOST2NETWORK_CONVERSION_NEEDED

// Host to Network 16-bit byte order conversion

#ifdef HOST2NETWORK_CONVERSION_NEEDED
#define H2N(x) ( ((x&0xFFFF)>>8) + ((x&0xFF)<<8))
#else
#define H2N(x) (x)
#endif

#ifdef HOST2NETWORK_CONVERSION_NEEDED
#define N2H(x) ( ((x&0xFFFF)>>8) + ((x&0xFF)<<8))
#else
#define N2G(x) (x)
#endif

/**
 * Convert IP address in text format to 4 byte int format (IPv4)
 */
int IPaddress2int(const char *IPaddress)
{
	int IPint = 0;
	inet_pton(AF_INET, IPaddress, &IPint ); // &(sa.sin_addr));
	return IPint;

	//printf("%s\n", str); // prints "192.0.2.33"

} // IPaddress2int

/**
 * Prints an IP address to stdout
 * @param int IPint IPv4 address
 * @returns -
 */
void printIPint(int IPint)
{
	struct sockaddr_in sa;
	//char str[INET_ADDRSTRLEN]; http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#inet_ntopman
	char str[16];

	// now get it back and print it
	inet_ntop(AF_INET, &(sa.sin_addr), str, 16);

	printf("%s\n", str); // prints "192.0.2.33"
} // printIPint

/**
 * Convert int to string
 * There should be better ways to do this.
 * @param int
 * @returns string
 */
string int2str( int i )
{
	string s;
	char temp[6];
	sprintf(temp, "%i", i);
	s = temp;
	return s;
}

/**
 * Convert int to string with formatting of 5 character ASPATH
 * There should be better ways to do this.
 * @param int
 * @returns string
 */
string int2ASPATHstr( int i )
{
	string s;
	char temp[6];
	sprintf(temp, "%05i", i);
	s = temp;
	return s;
}

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

int addRoute(int ASNAME, string ASPATH, int priority, int trust);
int addRoute(string ASNAME, string ASPATH, unsigned int mask, unsigned int prefixlen, unsigned int nextHop, unsigned int target, int priority);
int addRoute(string ASNAME, string ASPATH, unsigned int mask, unsigned int prefixlen, unsigned int nextHop, unsigned int target, int priority,
			unsigned int trust);
void printTableASPATH(string separator);
void printTableASPATH();
int ASPATHlength(string ASPATH);
int queryRoute(string destinationASNAME);
int queryRoute(int destinationASNAME);
string queryASPATHbyIP(int destinationIP);
int queryRouteIP(int destination);
void deleteRouteByASNAME( int ASNAME );
int deleteRoute();
int routeCount();
};

/*int routeTable::routeTable()
{
	//return 0;
}*/

/*
 * The int-version of addRoute
 * @param int			ASNAME
 * @param string		ASPATH
 * @param int priority	set to zero if no need
 * @param int trust		set to zero if no need
 * @returns routing table size
 */
int routeTable::addRoute(int ASNAME, string ASPATH, int priority, int trust)
{
	string ASNAMEstr;
	ASNAMEstr = int2ASPATHstr(ASNAME);
	return addRoute( ASNAMEstr, ASPATH,0,0,0,0,priority, trust);
}

/*
 * add route to routing table, with trust set to zero (it's OK to leave trust unset)
 * @param string ASNAME - MUST BE FORMATTED AS "%05i", i.e. 42 must be 00042
 * @param ...
 * @return routing table index for the route just added
 */
int routeTable::addRoute(string ASNAME, string ASPATH,
							unsigned int mask,
							unsigned int prefixlen,
							unsigned int nextHop, // IP of nexthop
							unsigned int target,  // IP of target (destination) packet
							int priority)
{
	routingItem_t rtRow;

	rtRow.ASNAME = ASNAME;
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
 * @param string ASNAME - MUST BE FORMATTED AS "%05i", i.e. 42 must be 00042
 * @param ...
 * @return routing table index for the route just added
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
 * Delete route with given ASNAME (all routes)
 * @param int ASNAME
 */
void routeTable::deleteRouteByASNAME( int ASNAME )
{
	//TODO: the actual deletion

}

/**
 *
 * @name    printTableASPATH
 * @brief  	Ouput routing table, with each line: ASNAME ASPATH PRIORITY
 * @ingroup routeTable
 *
 * It must search thorough the router table's items.
 *
 * @param [in] string separator value to be used in between the outputted items
 *
 * @retval no return value
 *
 */
void routeTable::printTableASPATH( string separator )
{
	int ri;
	routingItem_t rtRow; // to be used in iterations and comparisons
	//string separator = "\t";
	string paddedASPATH;

	cout << "Routing table:\n";
	cout << "ASNAME" << separator << "pri." << separator << "ASPATH" << endl;

	for (ri = 0; ri < rt.items; ri++)
	{
		rtRow = rt.ri[ri];
		cout << rtRow.ASNAME << separator << rtRow.priority << separator << rtRow.ASPATH << endl;
	}
} // ::printTableASPATH

void routeTable::printTableASPATH()
{
	printTableASPATH("\t");
}

/**
 *
 * @name    ASPATHlength
 * @brief  	Calculate the length of given ASPATH, return is integer
 * @ingroup routeTable
 *
 * Finds the next hop for given ASNAME, even when the given ASNAME is not in the neighbours.
 * It must search thorough the router tables items ASNAMEs.
 *
 * @param [in] (string ASPATH) ASPATH to be evaluated, as string
 *
 * @retval int Returns the number of ASes listed in ASPATH
 *
 * TO-DO: this should be a method of a routing table entry (routing table entries should be objects)
 */
int routeTable::ASPATHlength(string ASPATH)
{
	int wordCount = 1;
	if ( ASPATH.empty() )
		return 0;
	for (int i=0; i<ASPATH.length(); i++)
		if ( ASPATH[i]==' ' )
			wordCount++;
	return wordCount;
} // ::ASPATHlength

/**
 *
 * @name    queryRoute
 * @brief  	What is the next hop for given ASNAME
 * @ingroup routeTable
 *
 * Finds the next hop for given ASNAME, even when the given ASNAME is not in the neighbours.
 * It must search thorough the router tables items ASNAMEs.
 *
 * @param [in] (string destinationASNAME) target ASNAME as string
 *
 * @retval int ASNAME
 *
 */
int routeTable::queryRoute(string destinationASNAME)
{
	int ri;
	int resultDst = 0; // used for result destination IP, init with 0
	int resultPri = -9999999; // used for deciding the result // TO-DO: better lowest possible
	int resultLen =  9999999; // the length of the result NextHop's ASPATH (TO-DO MAXINT)
	// iterate rt
	//printf("route items: %i\n", rt.items);
	routingItem_t rtRow; // to be used in iterations and comparisons
	routingItem_t rtRowBest; // Best match saved here
	int match_found = 0; // indicates if match is already found or not \to-do { match_found could be avoided if rtRowBest is null until first match is found}++

	string paddedASPATH;
	string paddedDestinationASNAME;
	paddedDestinationASNAME = " " + destinationASNAME + " ";

	for (ri = 0; ri < rt.items; ri++)
	{
		rtRow = rt.ri[ri];

		// calculate if match found for destination in route table entries ASPATH

		// simple implementation (TO-DO: make this wicked fast)
		paddedASPATH = " " + rtRow.ASPATH + " "; // passedASPATH is now ASPATH with extra spaces

		if ( paddedASPATH.find( paddedDestinationASNAME ) != std::string::npos ) //search ASPATH with space as separator
		{
			// matching AS in the ASPATH is found!

			// is the new match better than existing

			// test2: is the new match ASPATH configured with bigger priority
			// TO-DO: priority is possible to set only for router table line (= per neighbour), not for each AS separately
			if (VERBOSE>2) printf("V3: ASPATH match ASPATH:%s\n", rtRow.ASPATH.c_str());
			if (VERBOSE>3) cout << "V4: MATCHING POSITION:"
					<< paddedASPATH.find( paddedDestinationASNAME )
					<< endl;
			if ( resultPri < rtRow.priority )
			{
				if (VERBOSE>3) printf("V4: ASPATH test2 match! pri old -> new: %i %i, ASPATH:%s\n", resultPri, rtRow.priority, rtRow.ASPATH.c_str());
				resultDst = rtRow.nextHop;
				resultPri = rtRow.priority;
				resultLen = ASPATHlength ( rtRow.ASPATH );
				rtRowBest = rtRow; // TO-DO: use this, not result*, NB! copy vs. pointer
			}
			else
				// test1: is the new match ASPATH length smaller
				if ( resultLen > ASPATHlength( rtRow.ASPATH ) )
				{
					if (VERBOSE>3) printf("V4: ASPATH test1 match! len old -> new: %i %i, ASPATH:%s\n", resultLen, ASPATHlength( rtRow.ASPATH ),rtRow.ASPATH.c_str() );
					resultDst = rtRow.nextHop;
					resultPri = rtRow.priority;
					resultLen = ASPATHlength( rtRow.ASPATH );
					rtRowBest = rtRow; // TO-DO: use this, not result*, NB! copy vs. pointer
				}
		}
	}

	//return resultDst;
	//std::string::size_type st;
	string s;
	int i;
	//i = stoi( rtRowBest.ASNAME, nullptr, 10 ); // why not working: http://www.cplusplus.com/reference/string/stoi/
	i = atoi( rtRowBest.ASNAME.c_str() );
	// return rtRowBest.ASNAME;
	return i; // returns ASNAME as int
}// ::queryRoute(string

/**
 * @returns the first ASNAME of ASPATH that has the given parameter
 */
int routeTable::queryRoute(int destinationASNAME)
{
	// convert paramater to string and call queryNextHop with that string

	string s;
	char temp[6];
	//s = itos( rtRowBest.ASNAME.c_str() );// why not working?
	s = "" + destinationASNAME;
	// itoa(destinationASNAME, temp, 10); // oh this rquires stdlib.h?
	sprintf(temp, "%i", destinationASNAME); // this to, but is standard #include <stdio.h>
	s = temp;
	return queryRoute( s );
} // ::queryRoute(int)


/**
 *
 * @name    queryASPATHbyIP
 * @brief  	What is the ASPATH for given neighbor IP address
 * @ingroup routeTable
 *
 * What is the ASPATH for given neighbor IP address
 *
 * @param [in] (int destinationIP) target router IP address in int format
 *
 * @retval string Returns ASPATH for given neighbour router, or NULL if there is a problem deciding the ASPATH
 *
 * TODO: actual implementation - now only "dummy" implementation, now gives the first item ASPATH in routing table for testing purposes
 */
string routeTable::queryASPATHbyIP(int destinationIP)
{
	if ( rt.items > 0)
		return rt.ri[0].ASPATH; // initially just sent any valid ASNAME from the RT
	else
	{
		return NULL;
	}
} // ::queryASPATHbyIP


/**
 * returns the AS IP address to send the packet to, for the packet that is on route to destination
 * return -1 on error (his should be rare situation. There should always be a default route, unless you are on the very top.)
 */
int routeTable::queryRouteIP(int destination)
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
			match_found++;
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
			// match_found++; // do not count matches twice!
		}
		cout << endl;
	} // if

	// if some destination was determined by the routing table, return it:
	if ( match_found > 0 )
		return resultDst;
	else
	{
		// if no match could be found, return something, if possible:
		if ( rt.items > 0)
			return rt.ri[0].nextHop; // in case of trouble, return some valid AS IP from the RT
		else
			return 0; // if all fails, send "0" -- TO-DO: some error code should be emitted
	}
} // ::queryRouteIP

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

