/**
 * SystemV2
 *
 * A network simulation system for BGP action with routers and packets
 *
 * -- may be we can just simulate network traffic and router action --
 * -- & forget about actual networking stuff that is provided by OS --
 *
 */


/*
 * Packet
 */
class Packet {
	Packet() {;}
	~Packet() {;};
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
	~Network(){};// Destructor

	void addRouter( Router router );
	void removeRouter( Router router );
	void transferPacket(Router targetRouter, Router senderRouter, Packet packet); // could be just ASNAME instead of Router?

	/*
	 * Add router to network
	 */
	void Network::addRouter( Router router ) {
		// add (link) to Router
	}

	void Network::removeRouter( Router router ) {
		// add (link) to Router
	}

}; // class Network



int main(void)
{

}
