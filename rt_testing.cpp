#include "rt.cpp"

/**
 * Testing unit for rt.cpp
 * Compile example: g++ -o rt_testing rt_testing.cpp -std=c++0x
 */

int main(void)
{
	routeTable myRouteTable; // create and initiaize route table

	// set up some routes for testing (these may do not make sense now):
	//          .addRoute(ASID, ASPATH, mask, prefixlen,nextHop, target, int priority)
	string ASPATH = "1 2 4 3"; string ASID ="12345";
	myRouteTable.addRoute(ASID, ASPATH, 0x000F, 	8, 	0x8844,  0x2344, 1);
	ASID = "12345"; ASPATH = "1 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x000F, 8, 0x0F00, 0x2344,1);
	ASID = "00042"; ASPATH = "1 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x000F, 8, 0xFFF0, 0x2344,2);
	ASID = "20500"; ASPATH = "1";
	myRouteTable.addRoute(ASID, ASPATH, 0xFFFF, 0, 0x1234, 0xFFFF,0);

	// set up trusted route (with trust parameter):
	myRouteTable.addRoute(ASID, ASPATH, 0xFFFF, 0, 0x1234, 0xFFFF,0,1000);

	// test the other routines:

	// RouteCount:
	cout << "Current routeTable route count: " << myRouteTable.routeCount() << endl;

	// routeQuery:
	//cout << "Route query result for X: " << myRouteTable.queryRoute(0x000F) << "." << endl;
	printf( "Route query result for X: %x\n", myRouteTable.queryRoute(0x000F));

	// routeDelete:

	return 0;
}
