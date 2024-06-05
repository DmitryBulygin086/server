#include "server.h"
#include "common.h"

int main(void)
{
    int listener;   // Listener socket descriptor
    int newfd;  // Newly accept()ed socket descriptor
    struct sockaddr_storage remoteaddr; // Client address
    socklen_t addrlen; // Here we store the size of the client address

    char buf[BUFFER_SIZE]; // Buffer for client data

    char remoteIP[INET6_ADDRSTRLEN]; // Here we store the client's IP address

    // Start off with room for 5 connections
    // (We'll realloc as necessary)
    int fd_count = 0; 
    int fd_size = 5;
    struct pollfd *pfds = malloc(fd_size * sizeof(*pfds)); // Allocate memory for fd_size pollfd structures

    // Set up and get a listening socket
    listener = get_listener_socket();

    if (listener == -1) {
        handle_error("Getting listener socket");
    }

    // Add the listener to set
    pfds[0].fd = listener;
    pfds[0].events = POLLIN; // Report ready to read on incoming connection

    fd_count++; // listener was added
    
    puts("Server: waiting for incoming connections...");
    // Main loop
    for(;;) {
        int poll_count = poll(pfds, fd_count,
                              -1); // Timeout -1 means that poll() will block in indefinitely 
                                           // until an event occurs on one of the file descriptors 
                                           // which are stored in pfds.

        if (poll_count == -1) {
            handle_error("poll");
        }

        // Run through the existing connections looking for data to read
        for(int i = 0; i < fd_count; i++) 
        {
            // Check if someone is ready to read
            if (pfds[i].revents & POLLIN) { // revents represents the events that occured on the file descriptor.
                
                // If listener is ready to read, handle new connection
                if (pfds[i].fd == listener) 
                {
                    addrlen = sizeof remoteaddr;
                    // store a new socket descriptor in newfd which is returend by accept()
                    newfd = accept(listener,  // listener wait for a new connection
                                   (struct sockaddr *)&remoteaddr, // set address of the connecting peer to remoteaddr
                                   &addrlen);

                  if (newfd == -1) 
                  {
                        perror("accept");
                  } 
                  else 
                  { 
                    // Add new socket descriptor to pfds
                        add_to_pfds(&pfds, newfd, &fd_count, &fd_size);

                        printf("Server: new connection from %s", inet_ntop(remoteaddr.ss_family,
                                                                                  get_in_addr((struct sockaddr*)&remoteaddr),
                                                                                 remoteIP, 
                                                                                  INET6_ADDRSTRLEN));
                        
                        printf(                               " on socket %d\n", newfd);        
                  }
                } 
                else // If not the listener, we're just a regular client
                { 
                    
                    int nbytes = recv(pfds[i].fd, buf, sizeof buf, 0);  // Read input bytes from client

                    int sender_fd = pfds[i].fd;


                    if (nbytes > 0) // We got data from a client
                    {
                        for(int j = 0; j < fd_count; j++) 
                        {
                            int recipient_fd = pfds[j].fd;

                            // Except the listener and ourselves
                            if (recipient_fd != listener && recipient_fd != sender_fd && recipient_fd & POLLIN) 
                            {
                                // Send to everyone
                                if (sendall(recipient_fd, buf, &nbytes) == -1) 
                                {
                                    perror("send");
                                }
                            }
                        }
                    }


                    else  // Got error or connection closed by client
                    {
                        if (nbytes == 0)  // If connection closed by client
                        {
                            printf("Server: socket %d hung up\n", sender_fd);
                        }
                        else // Got error
                        {
                            perror("recv");
                        }
                        // When a client disconnects from the server, 
                        // the server's poll() function detects the POLLHUP event on the corresponding file descriptor. 
                        // This event indicates that the client has closed its connection. 
                        // Upon detecting this event, the server closes the file descriptor using the close() function.
                        close(pfds[i].fd); 
                        // By closing the file descriptor, 
                        // the server releases the system resources associated with that client connection. 
                        // This ensures that the server can accept new connections and handle other clients 
                        // without running out of available file descriptors. 

                        // Delete file descriptor associated with i index in pfds array
                        del_from_pfds(pfds, i, &fd_count);
                    } 
                    
                } // END handle data from client
            } // END got ready-to-read from poll()
        } // END looping through file descriptors
    } // END for(;;)

    return 0;
}