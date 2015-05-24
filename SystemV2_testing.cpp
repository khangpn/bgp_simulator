/*
 * SystemV2_testing.cpp
 *
 * testing and example usage unit for SystemV2.cpp and it's IP packet handling functions
 *
 */

#ifndef VERBOSE
#define VERBOSE 2
#endif

#include "SystemV2.cpp"

int main(void)
{
#define IP_PACKET 4

	unsigned char payload[4]={'@','P','P',0};

	// create a new packet
	Packet p = Packet(IP_PACKET, 0, 0, 0, 0, 255, 6, 23100, 20500);
	p.Print();
	printf("Packet length: %i\n", p.getPacketLength());
	printf("Packet (header) checksum: %4x\n", p.getChecksum() );
	printf("Add 4 byte message to packet.\n");
	p.setMessage(payload, 4);
	printf("Packet length: %i\n", p.getPacketLength());
	printf("Packet (header) checksum: %4x\n", p.getChecksum() );

	unsigned char *buf; // buffer for icoming packet
	buf = p.serialize();

	printf("\n--\n\n"); // now test with serialized packet as bitstream in buffer buf

	Packet in = Packet(IP_PACKET, 0, 0, 0, 0, 31, 3, 0x1122, 0x3344); // yes, stoopid to initialize
	ip_header_t iph;
	iph = in.deserialize( buf, p.getPacketLength() );
	in.Print();
	printf("Packet length: %i\n", in.getPacketLength());
	printf("getMessage: %s.\n",in.getMessage()); // assumes that message happens to be null-terminated string!
	printf("Packet (header) checksum: %4x\n", in.getChecksum() );
	//p.recalculateChecksum();
	printf("Packet (header) checksum: %4x (again)\n", in.getChecksum() );
	printf("Decrease packet header TTL value.\n");
	in.setTTL( in.getTTL()-1 );
	//p.recalculateChecksum();
	printf("Packet (header) checksum: %4x (again)\n", in.getChecksum() );
	in.Print();

	//Code reserve for more testing:
	// 	buf = (unsigned char*)malloc( PACKET_MAX_LEN );
	//
	// 	p.recalculateChecksum(); // TODO should not be needed, checksum will soon be calculated automatically when needed!
	//


	// TODO make checksum inside header
	// TODO make proper checksum testing of header with checksum
	// TODO receive & inspect incoming packet (as buf char*)

	/*
	 * Usage:
	 *

$ g++ -std=c++11 -lpthread SystemV2_testing.cpp && ./a
Packet contents (in two lines after this) (format: {'index:hexvalue '}):
 0:45  1:00  2:14  3:00  4:00  5:00  6:00  7:00  8:ff  9:06 10:00 11:00
12:00 13:00 14:50 15:14 16:00 17:00 18:5a 19:3c
IP header field values in decimal:
ver:4. ihl:5. tos:0. len:   20. ide:0. fla:0. FrO:0. iph.ttl:255. iph.protocol:6.
iph.sourceip:20500. iph.destip:23100.
Packet (header) checksum: fda7

--

deserialized IP packet header:
Packet contents (in two lines after this) (format: {'index:hexvalue '}):
 0:45  1:00  2:14  3:00  4:00  5:00  6:00  7:00  8:ff  9:06 10:00 11:00
12:00 13:00 14:50 15:14 16:00 17:00 18:5a 19:3c
IP header field values in decimal:
ver:4. ihl:5. tos:0. len:   20. ide:0. fla:0. FrO:0. iph.ttl:255. iph.protocol:6.
iph.sourceip:20500. iph.destip:23100.
Packet (header) checksum: fda7

	 *
	 *
	 */
	;
}
