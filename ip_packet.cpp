/**
 * Calculate IP header checksum with "Internet checksum" algorithm
 * @param buf
 * @param size
 * @returns Internet checksum for size sized char buffer buf
 */
unsigned int IPchecksum(const char *buf, unsigned int size)
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
	return (~sum & 0xFFFF);
}
/*
 * Tests if the Internet checksum of input buffer is correct
 * @param buf
 * @param size
 * @return boolean
 */
unsigned int IPchecksumTest(const char *buf, unsigned int size)
{
	// lecture notes compare to 0xFFFF, but
	// correct packet internet checksum testing is 0xFFFF before inverse,
	// when inversed 0x0000 is correct result!
	return ( 0x0000 == IPchecksum(buf, size) );
}
