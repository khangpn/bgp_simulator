/**
 * Testing unit for routing table (for rt.cpp)
 *
 * Compile and use example:
 * 		g++ -o rt_test rt_testing.cpp -std=c++11 && (./rt_test | less )
 * @file   rt_testing.cpp
 * @Author Jussi Heino (juhehe@utu.fi)
 * @date   Feb-May 2015
 */

#define VERBOSE 0

#include "rt.cpp"

int main(void)
{

	printf("Testing unix for rt.cpp\n");
	printf("\t#define VERBOSE 4 for maximum debug output, 0 for minimum (now set to:%i)", VERBOSE );
	printf("\n");

	routeTable myRouteTable; // create and initiaize route table

	// set up some routes for testing (these may do not make sense now):
	//          .addRoute(ASID, ASPATH, mask, prefixlen,nextHop, target, int priority)
	string ASPATH = "12345 2 4 3"; string ASID ="12345";
	myRouteTable.addRoute(ASID, ASPATH, 0xF000, 8, 	0x8844, 0x2344, 1);
	ASID = "12346"; ASPATH = "12346 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0xFF00, 16, 0x1000, 0x2344, 1);
	ASID = "42"; ASPATH = "42 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0xFF00, 16, 0x2000, 0x2344, 2);


	// test cases for routing decisions: different prefixlen & diff. pri.
	ASID = "00500"; ASPATH = "500 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x0000FFFF, 16, 0x22550000, 0x55555555, 2);
	// let's see if the program chooses not the first or last matching item,
	// but the one with largest priority (3 in this case):
	ASID = "00502"; ASPATH = "502 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x0001FFFF, 17, 0x22550022, 0x55555555, 2);
	ASID = "00503"; ASPATH = "503 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x0001FFFF, 17, 0x22550033, 0x55555555, 3);
	ASID = "00501"; ASPATH = "501 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x0001FFFF, 17, 0x22550011, 0x55555555, 1);

	ASID = "10500"; ASPATH = "10500 3";
	myRouteTable.addRoute(ASID, ASPATH, 0xFFFFFFFF,  0,	0x11223344, 0xFFFFFFFF, 0);

	// set up trust-enabled route (with trust parameter):
	myRouteTable.addRoute(ASID, ASPATH, 0xFFFFFFFF,  0, 0x11223344, 0xFFFFFFFF, 0, 1000);
	myRouteTable.addRoute(1001, "1001 1 1 2 4", 0, 0);
	myRouteTable.addRoute(1002, "1002 1 1 2 4", 1, 0);
	myRouteTable.addRoute(1003, "1003 1 1 2 4", 0, 1);
	myRouteTable.addRoute(1004, "1004 1 1 2 4", 10, 1);
	myRouteTable.addRoute(1005, "1005 1 1 4", 10, 1);
	myRouteTable.addRoute(1006, "1006 1 1 4", 10, 0);

	// test the other routines:

	// print initial route table:
	myRouteTable.printTableASPATH( "\t|" );

	// RouteCount:
	cout << "Current routeTable route count: " << myRouteTable.routeCount() << endl;

	// routeQuery:
	//cout << "Route query result for X: " << myRouteTable.queryRoute(0x000F) << "." << endl;

	//int mydest = 0x55555555;
	//printf( "NextHop query result for %x: %x\n", mydest, myRouteTable.queryRoute( mydest ) );
	// should print  0x22550033 for dest 0x55555555 (pri is 3)

	//int mydest = 0x55555555;
	cout << "ASPATH query result for \"2\"" << ": " << myRouteTable.queryNextHop( "2" ) << endl;
	cout << "ASPATH query result for 2" << ": " << myRouteTable.queryNextHop( 2 ) << endl;
	cout << "ASPATH query result for 3" << ": " << myRouteTable.queryNextHop( 3 ) << endl;
	cout << "ASPATH query result for 503" << ": " << myRouteTable.queryNextHop( 503 ) << endl;

	string ASPATHtestString; int ASPATHtestResult;
	ASPATHtestString = "3 56 3 20500"; ASPATHtestResult=4;
	printf("ASPATHlength testing: %s, %i (%i)\n", ASPATHtestString.c_str(), myRouteTable.ASPATHlength(ASPATHtestString), (ASPATHtestResult==myRouteTable.ASPATHlength(ASPATHtestString)) );
	ASPATHtestString = "3 3 3 3"; ASPATHtestResult=4;
	printf("ASPATHlength testing: %s, %i (%i)\n", ASPATHtestString.c_str(), myRouteTable.ASPATHlength(ASPATHtestString), (ASPATHtestResult==myRouteTable.ASPATHlength(ASPATHtestString)) );
	ASPATHtestString = ""; ASPATHtestResult=0;
	printf("ASPATHlength testing: %s, %i (%i)\n", ASPATHtestString.c_str(), myRouteTable.ASPATHlength(ASPATHtestString), (ASPATHtestResult==myRouteTable.ASPATHlength(ASPATHtestString)) );
	ASPATHtestString = "1"; ASPATHtestResult=1;
	printf("ASPATHlength testing: %s, %i (%i)\n", ASPATHtestString.c_str(), myRouteTable.ASPATHlength(ASPATHtestString), (ASPATHtestResult==myRouteTable.ASPATHlength(ASPATHtestString)) );
	ASPATHtestString = "3 56 3 20500 1 1 1 1 1"; ASPATHtestResult=9;
	printf("ASPATHlength testing: %s, %i (%i)\n", ASPATHtestString.c_str(), myRouteTable.ASPATHlength(ASPATHtestString), (ASPATHtestResult==myRouteTable.ASPATHlength(ASPATHtestString)) );

	// routeDelete:
	// not yet

	return 0;
}
