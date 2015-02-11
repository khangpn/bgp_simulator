#include <iostream>
#include <cstring>      // Needed for memset
#include <sys/socket.h> // Needed for the socket functions
#include <netdb.h>      // Needed for the socket functions
#include <unistd.h>

void bgp_send(char *port, char *message)
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

  host_info.ai_family = AF_UNSPEC;     // IP version not specified. Can be both.
  host_info.ai_socktype = SOCK_STREAM; // Use SOCK_STREAM for TCP or SOCK_DGRAM for UDP.

  // Now fill up the linked list of host_info structs with google's address information.
  status = getaddrinfo("localhost", port, &host_info, &host_info_list);
  // getaddrinfo returns 0 on succes, or some other value when an error occured.
  // (translated into human readable text by the gai_gai_strerror function).
  if (status != 0)  std::cout << "getaddrinfo error" << gai_strerror(status) << std::endl;
  /* ========== END ========== */
    
  while(true)
  {
    /* Setting up socket */
    std::cout << "Creating a socket..."  << std::endl;
    int socketfd ; // The socket descripter
    socketfd = socket(host_info_list->ai_family, host_info_list->ai_socktype, 
    host_info_list->ai_protocol);
    // If the socket is errorous, retry in 2 seconds
    if (socketfd == -1) {
      std::cout << "socket error " ;
      close(socketfd);
      sleep(2);
      continue;
    }
    /* ========== END ========== */

    /* Connect to the port */
    std::cout << "Connect()ing..."  << std::endl;
    status = connect(socketfd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    // If the connection can not be established, retry in 2 seconds
    if (status == -1) {
      std::cout << "connect error" ;
      close(socketfd);
      sleep(2);
      continue;
    }
    /* ========== END ========== */

    /* Sending message */
    std::cout << "send()ing message to:" << port << "..."  << std::endl;
    char msg[1000];
    std::cin >> msg;
    int len;
    ssize_t bytes_sent;
    len = strlen(msg);
    bytes_sent = send(socketfd, msg, len, 0);
    /* ========== END ========== */

    /* Receiving message */
    std::cout << "Waiting to recieve data..."  << std::endl;
    ssize_t bytes_recieved;
    char incoming_data_buffer[1000];
    bytes_recieved = recv(socketfd, incoming_data_buffer,1000, 0);
    // If no data arrives, the program will just wait here until some data arrives.
    if (bytes_recieved == 0) std::cout << "host shut down." << std::endl ;
    if (bytes_recieved == -1)std::cout << "recieve error!" << std::endl ;
    std::cout << bytes_recieved << " bytes recieved :" << std::endl ;
    std::cout << incoming_data_buffer << std::endl;
    /* ========== END ========== */

    close(socketfd);
  }

  /* Closing connection */
  //std::cout << "Receiving complete. Closing socket..." << std::endl;
  //freeaddrinfo(host_info_list);
  //close(socketfd);
  /* ========== END ========== */
}