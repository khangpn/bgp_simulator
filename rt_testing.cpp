#include "rt.cpp"

/**
 * Testing unit for rt.cpp
 * Compile example: g++ -o rt_testing rt_testing.cpp -std=c++0x
 */

int main(void)
{
	routeTable myRouteTable; // create and initiaize route table

	// set up some routes for testing (these may do not make sense now):
	string ASPATH = "1 2 4 3"; string ASID ="10000";
	myRouteTable.addRoute(ASID, ASPATH, 0x000F, 0x8844, 0x2344,1);
	ASID = "00042"; ASPATH = "1 4 2 3";
	myRouteTable.addRoute(ASID, ASPATH, 0x000F, 0xFFF0, 0x2344,2);
	ASID = "20500"; ASPATH = "1";
	myRouteTable.addRoute(ASID, ASPATH, 0xFFFF, 0x1234, 0xFFFF,0);

	// test the other routines:

	// RouteCount:
	std::cout << "Current routeTable route count: " << myRouteTable.routeCount() << std::endl;

	// routeDelete:

	return 0;
}
