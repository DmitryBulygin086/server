# server
This is a server which allow client to exchange their messages.

# How to use this server
1. Clone this repository to your storage.
2. Navigate to the src folder of this repository in your terminal.
3. To compile server source code type a following command:
   
make server

5. Then run the server executable file:
   
./server
   
Now listener socket is ready to listen for incomming connections.

# Connecting to the server via telnet client (my own client is not ready).
To connect to the server you need to know your IPv4 address.
Open your terminal and type the following command:

ifconfig

Now look at inet raw.
Copy the sequence of numbers to the right of inet.
This is your IPv4 address.

Now when you know your IPv4 address, you can connect to the server via the following command:

telnet [YOUR IPv4 ADDRESS] [PORT NUMBER OF THE SERVER]

In this case, port number of the server is 8080 (look at the common.h file),
but you can change it by adding the following line to command.h file:

#define PORT [DESIRED PORT NUMBER]

When looking for port number, check the IANA list.(https://www.iana.org/assignments/service-names-port-numbers)

