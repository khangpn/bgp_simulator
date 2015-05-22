/**
 * SystemV2
 *
 * A network simulation system for BGP action with routers and packets
 *
 * -- may be we can just simulate network traffic and router action --
 * -- & forget about actual networking stuff that is provided by OS --
 *
 */

#include "ip_packet.cpp"

#include <stdio.h>
#include <stdlib.h> // for malloc


/*
 * Packet
 */
class Packet
{

#define PACKET_MAX_LEN 16384
unsigned char *buf = NULL;
unsigned int packet_size = 0;
unsigned int header_size = 0;

public:
	Packet() {;};
	~Packet() {;};

	Packet(int IP_PACKET, int TOS, int Identification, int Flags, int FO, int TTL, int Protocol, unsigned int srcIP , unsigned int dstIP)
	{
		struct ip_header {
			unsigned char	iph_ihl;
			unsigned char	iph_ver;
			unsigned char		iph_tos;
			unsigned short int	iph_len;
			unsigned short int	iph_ident;
			unsigned char		iph_flag;
			unsigned short int	iph_offset;
			unsigned char	iph_ttl;
			unsigned char	iph_protocol;
			unsigned short int	iph_chksum;
			unsigned int	iph_sourceip;
			unsigned int	iph_destip;
		};
		unsigned int iph_ihl = 5; // default and minimum header length of five 32b long words
		unsigned int iph_ver = 4; // 4 for IPv4
		unsigned int iph_tos	= 0; // TOS
		unsigned short iph_len	= 20; // length of packet, 20 bytes is minimum (5*4 bytes is just the header)
		unsigned int iph_ident	= Identification;
		unsigned int iph_flag	= Flags;
		unsigned int iph_offset	= FO; // Fragmentation Offset (0 for first in fragmented sequence)
		unsigned int iph_ttl	= TTL; // Time-To-Live
		unsigned int iph_protocol	= 6; // 6=TCP, but what is good here TODO
		unsigned int iph_chksum	= 0; // calculated, zero for calulation phase
		unsigned int iph_sourceip	= dstIP;
		unsigned int iph_destip		= srcIP;

		header_size = iph_len;
		packet_size = header_size;

		// construct a IP packet bitstream:

		unsigned char byte;
		int i = 0;
		buf = (unsigned char*)malloc(iph_len);
		if (buf == NULL) { printf("### ERROR: memory allocation error."); exit(2); }

		// first byte
		byte = 0xFF & ( iph_ihl + ( iph_ver << 4 ) );
		buf[i++] = byte;

		buf[i++] = iph_tos << 2; // two lowest bits unused/reserved

		iph_len = N2H(iph_len);
		buf[i++] = iph_len >>8; // Length. NB. Endianness is corrected.
		buf[i++] = iph_len & 0xFF;

		buf[i++] = Identification;

		buf[i++] = (Flags << 5 ); // +5 bits of FO
		buf[i++] = 0;				// last 8 bits of FO ( TODO FO assumed to be always zero)

		buf[i++] = TTL & 0xFF;
		buf[i++] = Protocol & 0xFF;

		buf[i++] = 0; // header checksum
		buf[i++] = 0;

		// TODO endianness and other
		buf[i++] = dstIP >>24 ;
		buf[i++] = (dstIP >>16) & 0xFF;
		buf[i++] = (dstIP >>8) & 0xFF;
		buf[i++] = (dstIP >>0) & 0xFF;

		// TODO endianness and other
		buf[i++] = srcIP >>24 ;
		buf[i++] = (srcIP >>16) & 0xFF;
		buf[i++] = (srcIP >>8) & 0xFF;
		buf[i++] = (srcIP >>0) & 0xFF;
	};

	/*
	 * methods
	 */
	void Print();
	unsigned short getChecksum();
	Packet deserialize();
	unsigned char *serialize(); // the checksum will be recalculated
	unsigned char *getMessage(); // the message payload of Packet
	void setMessage( unsigned char * message);
	void setTTL( int newTTL); // will set the IP header TTL value
};

void Packet::Print()
{
	printf("Packet contents:\n");
	for(int i =0; i<20; i++)
	{
		printf("%2i:%02x ", i, (unsigned char)buf[i] );
	}
	printf("\n");
}

unsigned short Packet::getChecksum()
{
	return IPchecksum((unsigned char *)buf, header_size);
}

/*
 * Router
 */
class Router {
	// Router definition

	Router(int ASNAME, int IP, int defaultGW, int netmask) {
		// RouteTable rt;
	}

	~Router() {;} // Destructor

public:

	void receivePacket( Packet packet ){

	}
};

class Network {

	// Router *routers;
	// array (or map<ASNAME>) of Router (added here with addRouter)

public:
	Network(){};// Constructor
	/*
	 * create a basic IPv4 header
	 */
		~Network(){};// Destructor

	void addRouter( Router router );
	void removeRouter( Router router );
	void transferPacket(Router targetRouter, Router senderRouter, Packet packet); // could be just ASNAME instead of Router?


}; // class Network

/*
 * Add router to network
 */
void Network::addRouter( Router router ) {
	// add (link) to Router
}

void Network::removeRouter( Router router ) {
	// add (link) to Router

}
