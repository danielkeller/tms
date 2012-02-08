#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <errno.h>

#include "socket.h"
#include "buffer.h"

epoll_event Socket::EpollIter::events[EPOLL_MAX_E];

void diep(const char *s)
{
	perror(s);
	exit(1);
}

int Socket::EpollIter::operator*()
{
	return events[cur].data.fd;
}

bool Socket::EpollIter::readable()
{
	return events[cur].events & EPOLLIN;
}

bool Socket::EpollIter::tcp()
{
	int fd = **this;
	int type;
	size_t len = sizeof(type);
	getsockopt(fd, SOL_SOCKET, SO_TYPE, &type, &len);
	return type == SOCK_STREAM;
}

Socket::IP::IP(const char * name, unsigned short port)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, name, &address.sin_addr);
}

Socket::IP::IP(unsigned long addr, unsigned short port)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(addr);
}

unsigned short Socket::IP::port()
{
	return ntohs(address.sin_port);
}

unsigned long Socket::IP::addr()
{
	return ntohl(address.sin_addr.s_addr);
}

int Socket::epollCreate()
{
	int fd = epoll_create1(0);
	if (fd == -1)
		diep("epoll_create1");
	return fd;
}

void Socket::epollWatchRead(int epollfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1 && errno != EEXIST)
		diep("epoll_ctl"); //EEXIST check needed to watch same socket for different pids
}
	
void Socket::epollWatchWrite(int epollfd, int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	if (epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &ev) == -1 && errno != EEXIST)
		diep("epoll_ctl");
}

Socket::EpollIter Socket::epollWait(int epollfd)
{
	Socket::EpollIter eit;
	int nfds = epoll_wait(epollfd, eit.events, EPOLL_MAX_E, -1);
	if (nfds == -1)
		diep("epoll_wait");
	eit.num = nfds;
	return eit;
}

bool Socket::read(int fd, Buffer & b)
{
	static char buf[1024];
	int num = ::read(fd, buf, 1024);
	if (num < 0)
		return false;
	b.push(buf, num);
	return true;
}

int Socket::udpOpen(short port)
{
	sockaddr_in sa;
	int sock = socket(AF_INET, SOCK_DGRAM | SOCK_NONBLOCK, IPPROTO_UDP);
	if (sock == -1)
		diep("socket");
	
	int yes = 1;  //keeps socket from getting stuck if we crash
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &yes, sizeof(yes)) == -1)
    {
        close(sock);
        diep("setsockopt");
    }

	sa.sin_family = AF_INET;
	sa.sin_port = htons(port);
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	
	if (bind(sock, (sockaddr*)&sa, sizeof(sa))==-1)
		diep("bind");
	return sock;
}

