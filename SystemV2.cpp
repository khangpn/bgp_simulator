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

struct ip_header_t {
	unsigned int iph_ihl; // default and minimum header length of five 32b long words
	unsigned int iph_ver; // 4 for IPv4
	unsigned int iph_tos; // TOS
	unsigned short iph_len; // length of packet, 20 bytes is minimum (5*4 bytes is just the header)
	unsigned int iph_ident;
	unsigned int iph_flag;
	unsigned int iph_offset; // Fragmentation Offset (0 for first in fragmented sequence)
	unsigned int iph_ttl; // Time-To-Live
	unsigned int iph_protocol; // 6=TCP, but what is good here TODO
	unsigned int iph_chksum; // calculated, zero for calulation phase
	unsigned int iph_sourceip;
	unsigned int iph_destip;
};

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
unsigned char *message = NULL;

public:
	Packet() {;};
	~Packet() {;};

	Packet(int IP_PACKET, int TOS, int Identification, int Flags, int FO, int TTL, int Protocol, unsigned int srcIP , unsigned int dstIP)
	{
		ip_header_t iph;// = new ip_header_t; // I do not know C....

		iph.iph_ihl = 5; // default and minimum header length of five 32b long words
		iph.iph_ver = 4; // 4 for IPv4
		iph.iph_tos	= 0; // TOS
		iph.iph_len	= 20; // length of packet, 20 bytes is minimum (5*4 bytes is just the header)
		iph.iph_ident	= Identification;
		iph.iph_flag	= Flags;
		iph.iph_offset	= FO; // Fragmentation Offset (0 for first in fragmented sequence)
		iph.iph_ttl	= TTL; // Time-To-Live
		iph.iph_protocol	= 6; // 6=TCP, but what is good here TODO
		iph.iph_chksum	= 0; // calculated, zero for calulation phase
		iph.iph_sourceip	= dstIP;
		iph.iph_destip		= srcIP;

		header_size = iph.iph_len;
		packet_size = header_size;

		// construct a IP packet bitstream:

		unsigned char byte;
		int i = 0;
		buf = (unsigned char*)malloc(iph.iph_len);
		if (buf == NULL) { printf("### ERROR: memory allocation error."); exit(2); }

		// first byte
		byte = 0xFF & ( iph.iph_ihl + ( iph.iph_ver << 4 ) );
		buf[i++] = byte;

		buf[i++] = iph.iph_tos << 2; // two lowest bits unused/reserved

		iph.iph_len = N2H(iph.iph_len);
		buf[i++] = iph.iph_len >>8; // Length. NB. Endianness is corrected.
		buf[i++] = iph.iph_len & 0xFF;

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
	ip_header_t deserialize( char *buf, int size);
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
 * Receive incoming buffer to Packet and form IP header stucture
 * @returns ip_header_t or NULL if incoming packet is not acceptable as IPv4 header
 */
ip_header_t Packet::deserialize( char *buf, int size)
{
	ip_header_t iph;
	// TODO

	// step 1 check if IPv4 info is OK
	// step 2 check IHL
	// step 3 check length?
	// step 4 check checksum?
	// step 5 other? (let me see lecture notes)
	return iph;
}
unsigned char *Packet::serialize() // the checksum will be recalculated
{
	// TODO make sure checksum is updated
	return buf;
}
unsigned char *Packet::getMessage()  // the message payload of Packet
{
	return this->message;
}

void Packet::setMessage( unsigned char * message)
{
	this->message = message;
	// TODO update checksum
}

void Packet::setTTL( int newTTL ) // will set the IP header TTL value
{
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
