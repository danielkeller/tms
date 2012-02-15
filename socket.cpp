#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <cstdio>
#include <cstdlib>
#include <set>

#include "socket.h"
#include "buffer.h"

using namespace std;

epoll_event Socket::EpollIter::events[EPOLL_MAX_E];
	
const Socket::IP Socket::IP::null;

set<int> epollReads;
set<int> epollWrites;
int epollfd;

sigset_t empty_set, old_set;

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

void Socket::IP::set(sockaddr_in & sa)
{
	address = sa;
}

void Socket::IP::set(const char * name, unsigned short port)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	inet_pton(AF_INET, name, &address.sin_addr);
}

void Socket::IP::set(unsigned long addr, unsigned short port)
{
	address.sin_family = AF_INET;
	address.sin_port = htons(port);
	address.sin_addr.s_addr = htonl(addr);
}

unsigned short Socket::IP::port() const
{
	return ntohs(address.sin_port);
}

unsigned long Socket::IP::addr() const
{
	return ntohl(address.sin_addr.s_addr);
}

ostream& operator<< (ostream& os, const Socket::IP& addr)
{
	static char str[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(addr.address.sin_addr), str, INET_ADDRSTRLEN);
	
	return os << str << ':' << addr.port();
}

bool Socket::operator<(const IP l, const IP r)
{
	return l.address.sin_addr.s_addr < r.address.sin_addr.s_addr;
}

bool Socket::operator==(const IP l, const IP r)
{
	return !memcmp(&l.address, &r.address, sizeof(l.address));
}

void Socket::epollCreate()
{
	epollfd = epoll_create1(0);
	if (epollfd == -1)
		diep("epoll_create1");

	sigemptyset(&empty_set); //block all signals outside of epoll_wait
	sigprocmask(SIG_SETMASK, &empty_set, &old_set);
}

void Socket::epollWatchRead(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int ctl;
	if (epollReads.count(fd))
		return;
	else if (epollWrites.count(fd))
		ctl = EPOLL_CTL_MOD;
	else
		ctl = EPOLL_CTL_ADD;

	if (epoll_ctl(epollfd, ctl, fd, &ev) == -1)
			diep("epoll_ctl");

	epollReads.insert(fd);
}
	
void Socket::epollWatchWrite(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	int ctl;
	if (epollWrites.count(fd))
		return;
	else if (epollReads.count(fd))
		ctl = EPOLL_CTL_MOD;
	else
		ctl = EPOLL_CTL_ADD;

	if (epoll_ctl(epollfd, ctl, fd, &ev) == -1)
			diep("epoll_ctl");

	epollWrites.insert(fd);
}

void Socket::epollUnWatchRead(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLOUT;
	ev.data.fd = fd;
	int ctl;
	if (!epollReads.count(fd))
		return;
	else if (epollWrites.count(fd))
		ctl = EPOLL_CTL_MOD;
	else
		ctl = EPOLL_CTL_DEL;

	if (epoll_ctl(epollfd, ctl, fd, &ev) == -1)
			diep("epoll_ctl");

	epollReads.erase(fd);
}
	
void Socket::epollUnWatchWrite(int fd)
{
	struct epoll_event ev;
	ev.events = EPOLLIN;
	ev.data.fd = fd;
	int ctl;
	if (!epollWrites.count(fd))
		return;
	else if (epollReads.count(fd))
		ctl = EPOLL_CTL_MOD;
	else
		ctl = EPOLL_CTL_DEL;

	if (epoll_ctl(epollfd, ctl, fd, &ev) == -1)
			diep("epoll_ctl");

	epollWrites.erase(fd);
}

Socket::EpollIter Socket::epollWait()
{
	Socket::EpollIter eit;
	int nfds = epoll_pwait(epollfd, eit.events, EPOLL_MAX_E, -1, &old_set);
	if (nfds == -1 && errno != EINTR)
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

bool Socket::read(int fd, Buffer & b, Socket::IP & src)
{
	static char buf[1024];
	sockaddr_in sa;
	size_t len = sizeof(sa); 
	int num = recvfrom(fd, buf, 1024, 0, (sockaddr*)&sa, &len);
	if (num < 0)
		return false;
	b.push(buf, num);
	src.set(sa);
	return true;
}

bool Socket::write(int fd, Buffer & b)
{
	int num = ::write(fd, b.buffer(), b.length());
	if (num < 0)
		return false;
	b.pop(num);
	return b.length() == 0;
}

bool Socket::write(int fd, Buffer & b, Socket::IP dst)
{ 
	int num = sendto(fd, b.buffer(), b.length(), 0, (sockaddr*)&(dst.address), sizeof(sockaddr_in));
	if (num < 0)
		return false;
	b.pop(num);
	return b.length() == 0;
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

