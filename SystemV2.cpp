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
	unsigned int flags;
	unsigned int FO; // Fragmentation Offset (0 for first in fragmented sequence)
	unsigned int ttl; // Time-To-Live
	unsigned int protocol; // 6=TCP, but what is good here TODO
	unsigned int checksum; // calculated, zero for calulation phase
	unsigned int sourceip;
	unsigned int destip;
};
#define PACKET_MAX_LEN 16384

#include <stdio.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy(sic!)

/*
 * Packet
 *
 * IP header special notes: destination and source IP addresses are not translated from host 32-bit int format to network byte order and vice versa!
 */
class Packet
{

unsigned char *buf = NULL;

unsigned int packet_size = 0; // TODO not use this, use values iniph via getters and setters!
unsigned int header_size = 0; // TODO not use this, use values iniph via getters and setters!

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
		iph.flags	= Flags;
		iph.FO	= FO; // Fragmentation Offset (0 for first in fragmented sequence)
		iph.ttl	= TTL; // Time-To-Live
		iph.protocol	= 6; // 6=TCP, but what is good here TODO
		iph.checksum	= 0; // calculated, zero for calulation phase
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

		//iph.len = N2H(iph.len);
		buf[i++] = N2H(iph.len) >>8; // Length. NB. Endianness is corrected.
		buf[i++] = N2H(iph.len) & 0xFF;

		buf[i++] = N2H(iph.ident) >> 8;
		buf[i++] = N2H(iph.ident) & 0xFF;

		buf[i++] = (iph.flags << 5 ) + iph.FO >> 8; // upper 5 bits of 13 bits of FO
		buf[i++] = iph.FO & 0xFF; // last 8 bits of FO ( TODO here FO assumed to be always zero)

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
	void recalculateChecksum(); // expensive recalculation of checksum
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

	void setMessageLength( int size );
	int getMessageLength();
	void setChecksum( unsigned short checksum );
	void setMessage( unsigned char *message, int size );
	void setPacketLength( int len );
	int getPacketLength();
	int getHeaderLengthBytes();
	void setHeaderLengthBytes( int ihl_bytes );
	int getHeaderLengthValue();
	void setHeaderLengthValue( int ihl );

	void remove(); 				// remove packet (free memory)
};

void Packet::Print()
{
	printf("Packet contents (in two lines after this) (format: {'index:hexvalue '}):\n");
	for(int i =0; i<iph.len; i++)
	{
		printf("%2i:%02x ", i, (unsigned char)buf[i] );
		if ( (i>1) && (i%11==0) )
			printf("\n");
	}

	// print newline - but do not if printing of ipheader happened to already have just printed newline
	if ( iph.len%11)
		printf("\n");

	printf("IP header field values in decimal:\n");
	// print ip_headet_t iph
	printf("ver:%i. ", iph.ver);
	printf("ihl:%i. ", iph.ihl);
	printf("tos:%i. ", iph.tos);
	printf("len:%5i. ", iph.len);
	printf("ide:%i. ", iph.ident);
	printf("fla:%i. ", iph.flags);
	printf("FrO:%i. ", iph.FO);
	printf("iph.ttl:%i. ", iph.ttl);
	printf("iph.protocol:%i. \n", iph.protocol);
	printf("iph.sourceip:%i. ", iph.sourceip);
	printf("iph.destip:%i.\n", iph.destip);

}

void Packet::recalculateChecksum()
{
	unsigned short newChecksum = 0;
	unsigned char *tempBuf; // why unsigned char *tempBuf[PACKET_MAX_LEN] does not work?
	tempBuf = (unsigned char*)malloc( PACKET_MAX_LEN ); // allocate large enough buffer

	tempBuf = this->serialize();
	newChecksum = IPchecksum((unsigned char *)tempBuf, this->getHeaderLengthBytes());
	this->setChecksum( newChecksum );
}

unsigned short Packet::getChecksum()
{
	return this->iph.checksum;
}

void Packet::setChecksum( unsigned short checksum )
{
	this->iph.checksum = checksum;
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

	iph.tos = buf[1] >> 2;

	iph.len = N2H( buf[2] << 8 + buf[3] );

	iph.ident = buf[4] << 8 + buf[5];

	iph.flags = buf[6] >> 5; // flag bits: bit0=MF, bit1=DF, bit2=N/U

	iph.FO = N2H( ( buf[6] & 0x1F ) << 8 + buf[7] ); // TODO

	iph.ttl = buf[8];

	iph.protocol = buf[9];

	iph.checksum = N2H( buf[10] <<8 + buf[11] );

	iph.sourceip = (int)buf[12];
	iph.destip = (int)buf[16];

	if (error)
		printf("### ERROR: Packet::deserialize: error count:%i.\n", error);

	return iph;
}

/*
 * make the packet into actual sendable bitstream
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
 * Set packet's message (=payload) length
 */
void Packet::setMessageLength( int size )
{
	// the new size of packet is header length + message length
	setPacketLength( getHeaderLengthBytes() + size );
}

int Packet::getMessageLength()
{
	// the new size of packet is header length + message length
	return getPacketLength() - getHeaderLengthBytes();
}

/*
 * set the message (=payload) portion of packet
 * - does not copy data
 * - message is not included in checksum
 * - if changing message changes message size, the packet size length changes and header checksum must be changed
 */
void Packet::setMessage( unsigned char *message, int size )
{
	this->message = message;
	setMessageLength( size );
	setChecksum(0);
	// TODO update checksum
}

/*
 * Set packet length field in header to given value
 */
void Packet::setPacketLength( int len )
{
	iph.len = len;
}

int Packet::getPacketLength()
{
	return iph.len;
}

/*
 * Returns packet's header's length in units of bytes
 */
int Packet::getHeaderLengthBytes()
{
	return this->iph.ihl*4;
}

void Packet::setHeaderLengthBytes( int ihl_bytes )
{
	this->iph.ihl = ihl_bytes/4;
}

/*
 * Return packet's header's length in raw field value with the unit of 32-bit words
 */
int Packet::getHeaderLengthValue()
{
	return this->iph.ihl;
}

void Packet::setHeaderLengthValue( int ihl )
{
	this->iph.ihl = ihl;
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
