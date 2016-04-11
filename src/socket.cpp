/* socket.cpp
 *
 * Contains the wrapper functions for a TCP socket.
 */

#include "../include/socket.h"

Socket::Socket ()
{
}

Socket::Socket (int socketfd)
{
    socketFD = socketfd;
}

int Socket::connect (string host, int port)
{
    /* Creating the socket. */
    if ((socketFD = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        int errsv = errno;
        std::cerr << "Cannot Create Socket, Error ";
        std::cerr << errsv << "\n";
        exit (1);
    }
    /* Determining server's IP address. */
    stringstream s1;
    s1 << port;
    int status;
    addrinfo hints;
    addrinfo* servinfo;
    memset (&hints, 0, sizeof (hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    if ((status = getaddrinfo (host.c_str (), s1.str ().c_str (), &hints,
                               &servinfo)) != 0)
        return -1;

    /* Connecting to the server. */
    return ::connect (socketFD, servinfo->ai_addr, servinfo->ai_addrlen);
}

int Socket::send (string data)
{
    return ::send (socketFD, data.c_str (), data.length (), 0);
}

int Socket::send (char* data, size_t size)
{
    return ::send (socketFD, data, size, 0);
}

string Socket::recv (size_t len)
{
    char* recv_buf = new char[len + 1];
    int recv_len = ::recv (socketFD, recv_buf, len, 0);
    if (recv_len < 0)
        return string ("");
    recv_buf[recv_len] = '\0';
    return string (recv_buf);
}

ssize_t Socket::recv (char* data, size_t len)
{
    return ::recv (socketFD, data, len, 0);
}

int Socket::bind (int port)
{
    /* Creating the socket. */
    if ((socketFD = socket (AF_INET, SOCK_STREAM, 0)) < 0)
    {
        int errsv = errno;
        std::cerr << "Cannot Create Socket, Error ";
        std::cerr << errsv << "\n";
        exit (1);
    }
    sockaddr_in myaddr;
    memset ((char*)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl (INADDR_ANY);
    myaddr.sin_port = htons (port);
    return ::bind (socketFD, (sockaddr *)&myaddr, sizeof (myaddr));
}

int Socket::listen (int backlog)
{
    return ::listen (socketFD, backlog);
}

Socket Socket::accept ()
{
    sockaddr_storage incoming;
    socklen_t addr_size;
    int tmp_fd = ::accept (socketFD, (sockaddr*)&incoming, &addr_size);
    return (Socket (tmp_fd));
}

string Socket::getSourceAddr ()
{
    char src[INET_ADDRSTRLEN];
    sockaddr_storage tmp;
    socklen_t addr_size = sizeof (sockaddr);
    getsockname (socketFD, (sockaddr*)&tmp, &addr_size);
    sockaddr_in* src_addr = (sockaddr_in*)&tmp;
    inet_ntop (AF_INET, &src_addr->sin_addr, src, sizeof (src));
    return string (src);
}

int Socket::getSourcePort ()
{
    sockaddr_in src_addr;
    socklen_t addr_size = sizeof (sockaddr);
    getsockname (socketFD, (sockaddr*)&src_addr, &addr_size);
    return ntohs (src_addr.sin_port);
}

string Socket::getDestAddr ()
{
    char dest[INET_ADDRSTRLEN];
    sockaddr_storage tmp;
    socklen_t addr_size = sizeof (sockaddr);
    getpeername (socketFD, (sockaddr*)&tmp, &addr_size);
    sockaddr_in* dest_addr = (sockaddr_in*)&tmp;
    inet_ntop (AF_INET, &dest_addr->sin_addr, dest, sizeof (dest));
    return string (dest);
}

int Socket::getDestPort ()
{
    sockaddr_in dest_addr;
    socklen_t addr_size = sizeof (sockaddr);
    getpeername (socketFD, (sockaddr*)&dest_addr, &addr_size);
    return ntohs (dest_addr.sin_port);
}

int Socket::close ()
{
    return ::close (socketFD);
}

Socket::~Socket ()
{
}
