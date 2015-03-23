#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>

using namespace std;

void bgp_listen(char *port)
{
  /* Setting up struct */
  int status;
  struct addrinfo host_info;       // The struct that getaddrinfo() fills up with data.
  struct addrinfo *host_info_list; // Pointer to the to the linked list of host_info's.

  // The MAN page of getaddrinfo() states "All  the other fields in the structure pointed
  // to by hints must contain either 0 or a null pointer, as appropriate." When a struct 
  // is created in C++, it will be given a block of memory. This memory is not necessary
  // empty. Therefor we use the memset function to make sure all fields are NULL.     
  memset(&host_info, 0, sizeof host_info);

  std::cout << "Setting up the structs..."  << std::endl;

  host_info.ai_family = AF_INET; // NO! lets use IPv4 -- AF_UNSPEC;     // IP version not specified. Can be both.
  host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.
    host_info.ai_flags = AI_PASSIVE;     // IP Wildcard

  // Now fill up the linked list of host_info structs with our localhost information
  // NULL for localhost
  status = getaddrinfo(NULL, port, &host_info, &host_info_list);
  // getaddrinfo returns 0 on succes, or some other value when an error occured.
  // (translated into human readable text by the gai_gai_strerror function).
  if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) << std::endl;
  /* ========== END ========== */
    
  /* Setting up socket */
  std::cout << "Creating a socket, port:" << port << "..."  << std::endl;
  int socketfd ; // The socket descripter
  socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, 
  host_info_list->ai_protocol);
  if (socketfd == -1)  std::cout << "socket error " ;
  /* ========== END ========== */

  /* Bind to the port */
  std::cout << "Binding socket..."  << std::endl;
  // we make use of the setsockopt() function to make sure the port is not in use.
  // by a previous execution of our code. (see man page for more information)
  int yes = 1;
  status = setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
  status = bind(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1)  std::cout << "bind error" << std::endl ;
  /* ========== END ========== */

  /* Listen to the port */
  int queue_len = 5; // How many cli_requests could be on hold
  std::cout << "Listen()ing for connections..."  << std::endl;
  status =  listen(socketfd, queue_len);
  if (status == -1)  std::cout << "listen error" << std::endl ;
  /* ========== END ========== */

  while(true)
  {
    /* Accept client connection */
    int new_sd;
    struct sockaddr_storage their_addr;
    socklen_t addr_size = sizeof(their_addr);
    new_sd = accept(socketfd, (struct sockaddr *)&their_addr, &addr_size);
    if (new_sd == -1)
    {
        std::cout << "listen error" << std::endl ;
    }
    else
    {
        std::cout << "Connection accepted. Using new socketfd : "  <<  new_sd << std::endl;
    }
    /* ========== END ========== */

    /* Receiving message */
    std::cout << "Waiting to recieve data..."  << std::endl;
    ssize_t bytes_recieved;
    char incomming_data_buffer[1000];
    bytes_recieved = recv(new_sd, incomming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
std::cout << bytes_recieved << " bytes recieved : ";// << std::endl ;
    incomming_data_buffer[bytes_recieved] = '\0';
    std::cout << incomming_data_buffer << std::endl;
/*JH
 if ( is_valid_IPv4_packet( incomming_data_buffer, bytes_recieved ) )
 switch BGP_type( incomming_data_buffer, bytes_recieved )
 {
 	 case KEEPALIVE:
 	 case UPDATE:
 	 case RESETetc:
 	 case default:
 }
*/

    /* ========== END ========== */

    /* Sending message */
    std::cout << "send()ing message..."  << std::endl;
    char msg[] = "HELLO CLIENT";
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(new_sd, msg, len, 0);
    /* ========== END ========== */

    close(new_sd);
  }

  /* Closing connection (uncomment for testing) */
    //std::cout << "Stopping server..." << std::endl;
    //freeaddrinfo(host_info_list);
    //close(new_sd);
    //close(socketfd);
  /* ========== END ========== */
}
