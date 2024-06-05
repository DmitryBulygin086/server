#include "server.h"
#include "common.h"


void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

int get_listener_socket(void)
{
    int listener;       // Listening socket descriptor
    int yes=1;          // For setsockopt() SO_REUSEADDR, below
    int rv;             // will hold return value of addrinfo()

    struct addrinfo hints, *res, *p;

   
    memset(&hints, 0, sizeof hints); // set hints structure to zero
    hints.ai_family = AF_UNSPEC; // hint for getaddrinfo(): getaddrinfo() will return socket addresses for any address family (either IPv4 or IPv6)
    hints.ai_socktype = SOCK_STREAM; // socket will be stream socket (TCP)
    hints.ai_flags = AI_PASSIVE; // when the AI_PASSIVE flag is set, getaddrinfo() will fill in the local IP address for us
    if ((rv = getaddrinfo(
                          NULL, // This indicates that the local host (the machine on which the program is running) should be used
                          PORT, // Macro that represents the port number on which the server will listen for incoming connections
                          &hints, // This is an address of a struct addrinfo that specifies the type of socket we want to create
                          &res)    // This is an address of pointer to a struct addrinfo. 
                                  // It will be filled in by getaddrinfo with the address information for the given host and service.
                          ) != 0)
    {
       handle_error("getaddrinfo:");
    }

    for(p = res; p != NULL; p = p->ai_next) 
    {
        listener = socket(p->ai_family, p->ai_socktype, p->ai_protocol); // store socket file descriptor in listener variable
        if (listener == -1) // if error
        {
            continue;
        }

        // Set the SO_REUSEADDR option on the listener socket 
        setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));

        // Associate the listener socket with a specific IP adress and port number
        if (bind(listener, p->ai_addr, p->ai_addrlen) < 0) {
            close(listener);
            continue;
        }

        break;
    }

    freeaddrinfo(res);   // freeing the memory allocated by the getaddrinfo() function

    // If p is NULL, it means that no valid address information was found
    if (p == NULL) {
        return -1;
    }

    // Setting the maximum number of pending connections that can be queued for the listening socket
    if (listen(listener, 10) == -1) {
        return -1;
    }

    return listener; // returns configured listening socket descriptor
}

void add_to_pfds(struct pollfd * pfds[], int newfd, int *fd_count, int *fd_size)
{
    // If we don't have room, add more space in the pfds array
    if (*fd_count == *fd_size) {
        *fd_size *= 2; // Double size

        // Reallocate memory for the pfds array with the new size
        *pfds = realloc(*pfds, sizeof(**pfds) * (*fd_size));
    }

    (*pfds)[*fd_count].fd = newfd; // Add new file descriptor to the pfds array
    (*pfds)[*fd_count].events = POLLIN; // This new file descriptor is ready to read

    (*fd_count)++;
}

void del_from_pfds(struct pollfd pfds[], int i, int *fd_count)
{
    // Copying the last element of the pfds array in the element at index i
    pfds[i] = pfds[*fd_count-1];

    (*fd_count)--;
}

void handle_error(const char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}

int sendall(int sockfd, char *buf, int *len)
{
    int total = 0;      // How many bytes we have sent
    int bytesleft = *len; // How many we have left to send
    int n;

    while (total < *len) {
        n = send(sockfd, buf + total, bytesleft, 0);
        if (n == -1) {break;}
        total += n;
        bytesleft -= n;
    }

    *len = total; // return number actually sent here

    return n == -1 ? -1 : 0; // return -1 on failure, 0 on success.
}