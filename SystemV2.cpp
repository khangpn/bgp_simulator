/**
 * class Packet
 *
 * also "SystemV2"
 *

To create a IP packet
1) create a new object Packet with constructor (see SystemV2_testing.cpp as an example)
2) add a message to the Packet with Packet::setMessage(unsigned char* buf, size)
3) checksum and packet length adjusted automatically in method setMessage
4) get raw Packet as a pointer with Packet::getMessage (getMessage allocates the memory)

NB.	There is no UDP and TCP header handling (currently).
	There is only IP header and message payload handling (currently).

 * This is also
 * a network simulation system for BGP action with routers and packets
 *
 * -- "Option B": THIS NOT THE CHOSEN WAYFOR THE PROJECT:
 * -- may be we can just simulate network traffic and router action --
 * -- & forget about actual networking stuff provided by OS --

 *
 */

// set to zero to kill all extra verbosity
#ifndef VERBOSE
#define VERBOSE 0
#endif


#include "ip_packet.cpp"

#include <stdio.h>
#include <stdlib.h> // for malloc
#include <string.h> // for memcpy(sic!)

/*
 * Packet
 * - IP header special notes: destination and source IP addresses are not translated from host 32-bit int format to network byte order and vice versa!
 * - relies on PACKET_MAX_LEN, rather than a value provided by the Packet
 */
class Packet
{

unsigned char *buf = NULL;

//unsigned int packet_size = 0; // TODO not use this, use values iniph via getters and setters!
//unsigned int header_size = 0; // TODO not use this, use values iniph via getters and setters!

//unsigned char *message = NULL;

ip_header_t iph;

public:

	void initA() {
		if ( buf == NULL )
		{
			buf = (unsigned char*)malloc( PACKET_MAX_LEN );
			//printf("initA(): buf is now %4x\n", buf);
		}
		iph.ihl = 5; // default and minimum header length of five 32b long words
		iph.ver = 4; // 4 for IPv4
		iph.tos	= 0; // TOS
		iph.len	= 20; // length of packet header, 20 bytes is minimum (5*4 bytes is just the header)
		iph.protocol	= 6; // 6=TCP, but what is good here TODO
		iph.checksum	= 0; // calculated, zero for calulation phase
	}

	Packet()
	{
		printf("jhdebug hellofrom _shorter_ constructor\n");
		//printf("buf is now %04x (::Packet,shorter)\n", buf);
		//Packet(4, 0, 0, 0, 0, 0, 0, 0, 0); // blank IPv4 packet
		initA();
		if (buf == NULL) { printf("### ERROR: memory allocation error. (::Packet,shorter)"); exit(2); }
	}; // Empty constructor

	// TODO "deserialize-constructor":
	// Packet(unsigned char *, int size)

	~Packet()
	{
		remove();
	};

	/*
	 * Constructor
	 * IP packet header only (add message with setMessage)
	 * - does not support optional headers
	 * - hard-coded protocol setting of 6=TCP
	 */
	Packet(int IP_PACKET, int TOS, int Identification, int Flags, int FO, int TTL, int Protocol, unsigned int srcIP , unsigned int dstIP)
	{
printf("jhdebug hellofrom longer constructor\n");
		initA();// buf = (unsigned char*)malloc( PACKET_MAX_LEN );
		if (buf == NULL) { printf("### ERROR: memory allocation error (::Packet,longer)."); exit(2); }

		iph.ihl = 5; // default and minimum header length of five 32b long words
		iph.ver = 4; // 4 for IPv4
		iph.tos	= 0; // TOS
		iph.len	= 20; // length of packet header, 20 bytes is minimum (5*4 bytes is just the header)
		iph.ident	= Identification;
		iph.flags	= Flags;
		iph.FO		= FO; // Fragmentation Offset (0 for first in fragmented sequence)
		iph.ttl		= TTL; // Time-To-Live
		iph.protocol	= 6; // 6=TCP, but what is good here TODO
		iph.checksum	= 0; // calculated, zero for calulation phase
		iph.sourceip	= dstIP;
		iph.destip		= srcIP;

		//header_size = iph.len;
		//packet_size = header_size;

//remove this duplicate code with ::deserialize: // TODO
/**/
		// construct an IP packet bitstream:

		unsigned char byte;
		int i = 0;
		//buf = (unsigned char*)malloc(iph.len);
		//if (buf == NULL) { printf("### ERROR: memory allocation error."); exit(2); }

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
/**/
		// TODO poor code to update checksum after all:
		int checksum =0;
		checksum = IPchecksum( this->buf, this->getHeaderLengthBytes() );
		this->setChecksum(checksum);

		// buf should be set up-to-date as after the constructor the situation should be the same as with after serialize()
		buf[10] = H2N(checksum) >> 8;
		buf[11] = H2N(checksum) & 0xFF;

printf("jhdebug iph.len:%i (at longer constr end)\n", iph.len);
};

	/*
	 * methods
	 */
	void Print();
	void recalculateChecksum(); // expensive recalculation of checksum
	unsigned short getChecksum();
	void deserialize( unsigned char *buf, int size);
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

void Packet::Print() // expects that the buf is filled
{
	//unsigned char* tempBuf = (unsigned char*)malloc( PACKET_MAX_LEN );
	//tempBuf = this->serialize();
	if ( buf == NULL ) { printf("#ERR ::Print buf is null\n"); exit(2); }
	printf("Packet contents (in two lines after this) (format: {'index:hexvalue '}):\n");
	for(int i =0; i<iph.len; i++)
	{
		printf("%2i:%02x ", i, (unsigned char)buf[i] );
		if ( (i>1) && (i%11==0) )
			printf("\n");
	}
	//free(tempBuf);

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

/*
 * Recalculate IP packet header checksum (update internal struct)
 * - packet header checksum must be updated every time header changes
 * - checksum can be calculated for the difference only, but this calculates for whole header
 * @params none
 * @returns none
 */
void Packet::recalculateChecksum()
{
	unsigned short newChecksum = 0;
//	unsigned char *tempBuf; // why unsigned char *tempBuf[PACKET_MAX_LEN] does not work?
	//tempBuf = (unsigned char*)malloc( PACKET_MAX_LEN ); // allocate large enough buffer

	// clear checksum field for computation of checksum
	this->setChecksum(0);

	// serialize and calculate new checksum
	//tempBuf = this->serialize();
	newChecksum = IPchecksum( buf, this->getHeaderLengthBytes() );

	// finish: clear reserved memory and set new checksum
	//free(tempBuf);
	this->setChecksum( newChecksum );
}

unsigned short Packet::getChecksum()
{
	this->recalculateChecksum();
	return this->iph.checksum;
}

void Packet::setChecksum( unsigned short checksum )
{
	this->iph.checksum = checksum;
}

/*
 * Receive incoming buffer to Packet and form IP header structure
 * - copies all data from incoming parameter buf
 * TODO: needs not return anything
 * @returns ip_header_t or NULL if incoming packet is not acceptable as IPv4 header
 */
void Packet::deserialize( unsigned char *bufIn, int size )
{
	int error = 0; // count errors
	unsigned char a;
	unsigned char b;
	a=bufIn[0];
printf("a:%0x\n", a);
b=buf[0];
printf("b:%0x\n", b);
	// Initial test, for minimum length requirement.
	// Without having a minimum amount of data the result cannot form a valid IPv4 header.
	if ( size < IP_HEADER_LENGTH_MINIMUM )
	{
		if (VERBOSE>1) printf("### ERROR: Packet::deserialize, size=%i<IP_HEADER_LENGTH_MINIMUM=%i\n", size, IP_HEADER_LENGTH_MINIMUM);
		error++;
	}

	if (VERBOSE>0) printf("deserialized IP packet header:\n");

	// uses optimal verification strategy: complete structure is formed first and validity verification is done afterwards

	iph.ver = bufIn[0] >> 4;
	iph.ihl = bufIn[0] & 0xF;

	iph.tos = bufIn[1] >> 2;

	iph.len = N2H( bufIn[2] << 8 + bufIn[3] );

	iph.ident = bufIn[4] << 8 + bufIn[5];

	iph.flags = bufIn[6] >> 5; // flag bits: bit0=MF, bit1=DF, bit2=N/U

	iph.FO = N2H( ( bufIn[6] & 0x1F ) << 8 + bufIn[7] ); // TODO

	iph.ttl = bufIn[8];

	iph.protocol = bufIn[9];

	iph.checksum = N2H( bufIn[10] <<8 + bufIn[11] );

	iph.sourceip = bufIn[12]<<24 + bufIn[13]<<16+ bufIn[14]<<8+bufIn[15];
	iph.destip = bufIn[16]<<24 + bufIn[17]<<16+ bufIn[18]<<8+bufIn[19];

	// the actual validity tests:

	if (iph.ver != 4) {
		if (VERBOSE>0) printf("### ERROR: Packet::deserialize, iph.ver=%i != 4\n", iph.ver);
		error++;
	}

	//TODO
	// 	X 	step 1 check if IPv4 info is OK
	// 	O 	step 2 check IHL
	// 	~ 	step 3 check length?
	// 	O 	step 4 check checksum?
	// 	O 	step 5 other? (let me see lecture notes)
	//  O   step 6 check the consistency with size from parameter and reported size in header

	if (error)
		printf("### ERROR: Packet::deserialize: error count:%i.\n", error);

	// TODO is this correct
	// TODO old message memory is not released?
//	if (this->message != NULL)
//		free(this->message);

//	this->message = (unsigned char*)malloc(iph.len);
//	memcpy(this->message, &buf[20], iph.len-iph.ihl*4 ); // TODO
	//message = &buf[20];

	memcpy(buf, bufIn, size); // copy everything into the internal buf, including the message portion

	//return iph;
}

/*
 * make the packet into actual sendable bitstream
 * get packet length with Packet::getPacketLength
 * allocates new memory and returns pointer to that
 */
unsigned char *Packet::serialize()
{
	/**/
			// construct an IP packet bitstream:

			unsigned char byte;
			int i = 0;
			//unsigned char *buf = (unsigned char*)malloc(iph.len); // allocate all new buf and mem

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

			buf[i++] = iph.ttl & 0xFF;
			buf[i++] = iph.protocol & 0xFF;

			buf[i++] = 0; // header checksum
			buf[i++] = 0;

			// TODO endianness and other
			buf[i++] = iph.destip >>24 ;
			buf[i++] = (iph.destip >>16) & 0xFF;
			buf[i++] = (iph.destip >>8) & 0xFF;
			buf[i++] = (iph.destip >>0) & 0xFF;

			// TODO endianness and other
			buf[i++] = iph.sourceip >>24 ;
			buf[i++] = (iph.sourceip >>16) & 0xFF;
			buf[i++] = (iph.sourceip >>8) & 0xFF;
			buf[i++] = (iph.sourceip >>0) & 0xFF;
	/**/

	// i should now be 20 (no optional headers)
	if (i!=20) { printf("### ERROR i != 20"); exit(1); }
	//memcpy(&buf[i], this->message, getMessageLength() );
	unsigned char* returnBuf = (unsigned char*)malloc( iph.len );
	memcpy(returnBuf, buf, getPacketLength() );

	return returnBuf;
}

/*
 * return the pointer to packet's message (=payload)
 */
unsigned char *Packet::getMessage() // the message payload of Packet
{
	return (unsigned char*)(this->buf + this->getHeaderLengthBytes());
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
 * set the message payload portion of packet
 * - DOES COPY DATA (old behaviour: does not copy data)
 * - message is not included in checksum
 * - if changing message changes message size, the packet size length changes and header checksum must be changed
 */
void Packet::setMessage( unsigned char *newMessage, int size )
{
	//this->message = message;
//	this->message = (unsigned char*)malloc(size); // allocate new mem
	unsigned char* message; // pointer to message portion of this->buf
	message = buf + getHeaderLengthBytes();
	memcpy(message, newMessage, size);

	setMessageLength( size );
	//this->recalculateChecksum();
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
 * - TODO check if this is really needed and also yields intended result
 */
void Packet::remove()
{
	free(this->buf);
}


// * // * // * // * // * // * // * // * // * // * // * // * // * //


/*
 * Router (unfinished router definition)
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

class Network // (unfinished network definition)
{

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
