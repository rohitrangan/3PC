/* socket.h
 *
 * Contains the wrapper functions for a TCP socket.
 */

#ifndef PC_SOCKET_H_
#define PC_SOCKET_H_

#include <cerrno>
#include <string>
#include <cstring>
#include <cstdlib>
#include <sstream>
#include <iostream>

#include <fcntl.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define BACKLOG     10
#define RECV_SIZE   1000

using namespace std;

class Socket
{
private:
    int socketFD;
    bool blockingEnabled;

public:
    Socket ();
    Socket (int sockfd);
    ~Socket ();
    int connect (string host, int port);
    int send (string data);
    int send (char* data, size_t size);
    string recv (size_t len);
    ssize_t recv (char* data, size_t len);
    int bind (int port);
    int listen (int backlog);
    Socket accept ();
    string getSourceAddr ();
    int getSourcePort ();
    string getDestAddr ();
    int getDestPort ();
    int close ();
    int enableBlocking ();
    int disableBlocking ();
};

#endif  /* 3PC_SOCKET_H_ */
