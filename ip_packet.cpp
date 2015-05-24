/*
 * ip_packet.cpp has C-style functions and definitions related to IP packet
 */

#include <stdio.h>

// for inet_pton-related usage examples:
#include <sys/types.h>

#include <arpa/inet.h>
#include <netinet/in.h>

// define for Intel-style processors:
#define HOST2NETWORK_CONVERSION_NEEDED

// Host to Network 16-bit byte order conversion
// - only use if hton and htonl etc. are not available
// - NB! Visual C++ and GCC offer their solutions to endianness conversion too, cf. http://stackoverflow.com/questions/105252/

#ifdef HOST2NETWORK_CONVERSION_NEEDED
#define H2N(x) ( ((x&0xFFFF)>>8) + ((x&0xFF)<<8))
#else
#define H2N(x) (x)
#endif

#ifdef HOST2NETWORK_CONVERSION_NEEDED
#define N2H(x) ( ((x&0xFFFF)>>8) + ((x&0xFF)<<8))
#else
#define N2H(x) (x)
#endif

#define IP_HEADER_LENGTH_MINIMUM 20

/**
 * Convert IP address in text format to 4 byte int format (IPv4)
 */
unsigned int IPaddress2int(const char *IPaddress)
{
	unsigned int IPint = 0;
	inet_pton(AF_INET, IPaddress, &IPint ); // &(sa.sin_addr));
	return IPint;

	//printf("%s\n", str); // prints "192.0.2.33"

} // IPaddress2int

/**
 * Prints an IP address to stdout
 * @param int IPint IPv4 address
 * @returns -
 */
void printIPint(int IPint)
{
	struct sockaddr_in sa;
	//char str[INET_ADDRSTRLEN]; http://beej.us/guide/bgnet/output/html/singlepage/bgnet.html#inet_ntopman
	char str[16];

	// now get it back and print it
	inet_ntop(AF_INET, &(sa.sin_addr), str, 16);

	printf("%s\n", str); // prints "192.0.2.33"
} // printIPint

/**
 * Calculate IP header checksum with "Internet checksum" algorithm
 * - does not change any bytes in given buffer (make the checksum field zero before calling if you like to)
 * @param buf
 * @param size
 * @returns "Internet checksum" (same result as algorithm described in RFC971) for size sized char buffer buf
 */
unsigned short IPchecksum(const unsigned char *buf, unsigned int size)
{
	unsigned int sum = 0;
	unsigned short uint16 = 0; // TODO could be integrated into summing in for?

	// checksum with 16-bit pieces
	for ( int i = 0; i < size >> 1; i++ )
	{
		// think about byte order
		uint16 = buf[i<<1]&0xFF;
		uint16 += buf[1+(i<<1)]<<8;
		sum += uint16;
	}

	// single byte at the end? (odd size) (not tested!)
	if ( size & 0x0001 )
	{
		sum += buf[size] &0xFF;
	}

	sum = (sum >> 16) + (sum &0xFFFF); // sum MSW and LSW

	sum = H2N(sum); // byte order swap here or elsewhere?
	return (unsigned short)(~sum & 0xFFFF);
}

/*
 * Tests if the Internet checksum of input buffer is correct
 * @param buf
 * @param size
 * @return boolean
 */
unsigned short IPchecksumTest(const unsigned char *buf, unsigned int size)
{
	/* lecture notes compare to 0xFFFF, but
	 * correct packet internet checksum testing is 0xFFFF before inverse,
	 * when inversed 0x0000 is correct result!
	 * (due to not making the checksum field zero before calling the checksum function)
	 *
	 */
	return ( 0x0000 == IPchecksum(buf, size) );
}
