#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <cstdio>
#include <cstdlib>

#include "socket.h"

void diep(const char *s)
{
	perror(s);
	exit(1);
}

int Socket::EpollIter::operator*()
{
	return events[cur].data.fd;
}

int Socket::epollCreate()
{
	int fd = epoll_create1(0);
	if (fd == -1)
		diep("epoll_create1");
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

EpollIter Socket::epollWait(int epollfd)
{
	EpollIter eit;
	int nfds = epoll_wait(epollfd, eit.events, EPOLL_MAX_E, -1);
	if (nfds == -1)
		diep("epoll_wait");
	eit.num = nfds;
	return eit;
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

