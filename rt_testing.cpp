#include "rt.cpp"

/*
 * Testing unit for rt.cpp
 * Compile example: g++ -o rt_testing rt_testing.cpp -std=c++0x
 */

int main(void)
{
routeTable myRouteTable;
string ASPATH = "1 2 3";
myRouteTable.addRoute(ASPATH, 0xFF00, 0xFFF0, 0x2344);
std::cout << "Current routeTable route count: " << myRouteTable.RouteCount() << std::endl;
return 0;
}
