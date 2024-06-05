#ifndef _SERVER_H_
#define _SERVER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <poll.h>



/* operation:      get version of Internet Protocol: IPv4 or IPv6                   */
/* preconditions:  sa points to structure which describes socket address            */
/* postconditions: returns IPv4 address or IPv6 address                             */
void * get_in_addr(struct sockaddr *sa);

/* operation:      set up and get a listening socket                                 */
/* preconditions:  none                                                              */
/* postconditions: function returns a configured socket that is ready to listen      */
int get_listener_socket(void);
 
/* operation:       add a new file descriptor to the set                              */
/* preconditions:   pfds[] is an array of pointers to the pollfd structures           */
/*                  newfd is a file descriptor which will be added                    */
/* postconditions:  newfd will be added to the array of pollfd structures             */
void add_to_pfds(struct pollfd *pfds[], int newfd, int *fd_count, int *fd_size);

/* operation:       remove an index from the set                                       */
/* preconditions:   pfds is an array of pollfd structures                              */
/*                  i is an index which will be removed                                */
/*                  fd_count is a pointer to the current count of file descriptors     */
/*                  in the pfds array                                                  */
/* postconditions:  file descriptor with index i will be removed from the pfds[] array */
void del_from_pfds(struct pollfd pfds[], int i, int *fd_count);

/* Operation: send all data via send function                                        */
int sendall(int sockfd, char *buf, int *len);


#endif