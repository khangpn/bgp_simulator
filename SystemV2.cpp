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
	unsigned int ihl; // default and minimum header length of five 32b long words
	unsigned int ver; // 4 for IPv4
	unsigned int tos; // TOS
	unsigned short len; // length of packet, 20 bytes is minimum (5*4 bytes is just the header)
	unsigned int ident;
	unsigned int flag;
	unsigned int offset; // Fragmentation Offset (0 for first in fragmented sequence)
	unsigned int ttl; // Time-To-Live
	unsigned int protocol; // 6=TCP, but what is good here TODO
	unsigned int chksum; // calculated, zero for calulation phase
	unsigned int sourceip;
	unsigned int destip;
};
#define PACKET_MAX_LEN 16384

#include <stdio.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy(sic!)

/*
 * Packet
 */
class Packet
{

unsigned char *buf = NULL;
unsigned int packet_size = 0;
unsigned int header_size = 0;
unsigned char *message = NULL;
ip_header_t iph;

public:
	Packet() {;};
	~Packet() {;};

	Packet(int IP_PACKET, int TOS, int Identification, int Flags, int FO, int TTL, int Protocol, unsigned int srcIP , unsigned int dstIP)
	{

		iph.ihl = 5; // default and minimum header length of five 32b long words
		iph.ver = 4; // 4 for IPv4
		iph.tos	= 0; // TOS
		iph.len	= 20; // length of packet, 20 bytes is minimum (5*4 bytes is just the header)
		iph.ident	= Identification;
		iph.flag	= Flags;
		iph.offset	= FO; // Fragmentation Offset (0 for first in fragmented sequence)
		iph.ttl	= TTL; // Time-To-Live
		iph.protocol	= 6; // 6=TCP, but what is good here TODO
		iph.chksum	= 0; // calculated, zero for calulation phase
		iph.sourceip	= dstIP;
		iph.destip		= srcIP;

		header_size = iph.len;
		packet_size = header_size;

		// construct a IP packet bitstream:

		unsigned char byte;
		int i = 0;
		buf = (unsigned char*)malloc(iph.len);
		if (buf == NULL) { printf("### ERROR: memory allocation error."); exit(2); }

		// first byte
		byte = 0xFF & ( iph.ihl + ( iph.ver << 4 ) );
		buf[i++] = byte;

		buf[i++] = iph.tos << 2; // two lowest bits unused/reserved

		iph.len = N2H(iph.len);
		buf[i++] = iph.len >>8; // Length. NB. Endianness is corrected.
		buf[i++] = iph.len & 0xFF;

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
	ip_header_t deserialize( unsigned char *buf, int size);
	unsigned char *serialize();		// the checksum will be recalculated
	unsigned char *getMessage();	// the message payload of Packet
	void setMessage( unsigned char * message);
	void setTTL( int ttl);	// will set the IP header TTL value
	int getTTL(); // will set the IP header TTL value
	void setDestip( int destip ); // will set the header value for destination IP
	int getDestip(); // will set the header value for destination IP
	void setSourceip( int sourceip ); // will set the header value for destination IP
	int getSourceip(); // will set the header value for destination IP

	void remove(); 				// remove packet (free memory)
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
 * Receive incoming buffer to Packet and form IP header structure
 * @returns ip_header_t or NULL if incoming packet is not acceptable as IPv4 header
 */
ip_header_t Packet::deserialize( unsigned char *buf, int size )
{
	ip_header_t iph;
	ip_header_t iph_temp; // temporary memory structure
	int error = 0; // count errors

	if ( size < IP_HEADER_LENGTH_MINIMUM )
		error++;

	// TODO

	// step 1 check if IPv4 info is OK
	// step 2 check IHL
	// step 3 check length?
	// step 4 check checksum?
	// step 5 other? (let me see lecture notes)

	memcpy(&iph, buf, 20); // 20 is ipheader minimum size, and the size of ip_header_t TODO make better

	// other non-byte-aligned separation // TODO

	// FIXME
	// TODO
	printf("deserialized IP packet header:\n");
	iph.ver = buf[0] >> 4;
	iph.ihl = buf[0] & 0xF;
	printf("ver:%i. ", iph.ver);
	printf("ihl:%i. ", iph.ihl);
	printf("iph.ttl:%i.\n", iph.ttl);

	if (error)
		printf("### ERROR: Packet::deserialize: error count:%i.\n", error);

	return iph;
}

/*
 *
 */
unsigned char *Packet::serialize() // the checksum will be recalculated
{
	// TODO make sure checksum is updated
	return buf;
}

/*
 * return the pointer to packet's message (=payload)
 */
unsigned char *Packet::getMessage()  // the message payload of Packet
{
	return this->message;
}

/*
 * set the message (=payload) portion of packet
 * - does not copy data
 */
void Packet::setMessage( unsigned char * message )
{
	this->message = message;
	// TODO update checksum
}

void Packet::setTTL( int ttl ) // will set the IP header TTL value
{
	this->iph.ttl = ttl;
}

int Packet::getTTL() // will set the IP header TTL value
{
	return this->iph.ttl;
}

void Packet::setDestip( int destip ) // will set the header value for destination IP
{
	this->iph.destip = destip;
}

int Packet::getDestip() // will set the header value for destination IP
{
	return this->iph.destip;
}

void Packet::setSourceip( int sourceip ) // will set the header value for destination IP
{
	this->iph.sourceip = sourceip;
}

int Packet::getSourceip() // will set the header value for destination IP
{
	return this->iph.sourceip;
}


/*
 * free allocated memory for packet
 */
void Packet::remove()
{
	free(this->message);
	free(this->buf);
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
