#include "SystemV2.cpp"

int main(void)
{
#define IP_PACKET 4

	Packet p = Packet(IP_PACKET, 0, 0, 0, 0, 255, 6, 23100, 20500);
	p.Print();
	//p.recalculateChecksum(); // TODO should not be needed, checksum will soon be calculated automatically when needed!
	printf("Packet (header) checksum: %4x\n", p.getChecksum() );

	ip_header_t iph;
	unsigned char *buf;
	buf = (unsigned char*)malloc( PACKET_MAX_LEN );

	printf("\n--\n\n");

	buf = p.serialize();
	iph = p.deserialize(buf, PACKET_MAX_LEN);
	p.Print();
	//p.recalculateChecksum(); // TODO should not be needed, checksum will soon be calculated automatically when needed!
	printf("Packet (header) checksum: %4x\n", p.getChecksum() );

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
