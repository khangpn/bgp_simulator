#include "SystemV2.cpp"

int main(void)
{
#define IP_PACKET 4

	Packet p = Packet(IP_PACKET, 0, 0, 0, 0, 255, 6, 23100, 20500);
	p.Print();
	printf("Packet (header) checksum: %4x\n", p.getChecksum() )
	// TODO make checksum inside header
	// TODO make proper checksum testing of header with checksum

	// TODO receive & inspect incoming packet (as buf char*)

	/*
	 * Usage:
	 *
$ g++ -std=c++11 -lpthread SystemV2_testing.cpp && ./a
Packet contents:
 0:45  1:00  2:14  3:00  4:00  5:00  6:00  7:ff  8:06  9:00 10:00 11:00 12:00 13:50 14:14 15:00 16:00 17:5a 18:3c 19:00
Packet (header) checksum: 4f56
	 *
	 *
	 */
	;
}
