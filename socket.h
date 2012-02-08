#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netinet/in.h>
#include <sys/epoll.h>
#include "common.h"

//abstraction layer for POSIX sockets
//contains error handling
namespace Socket
{
	//epoll_event[] wrapper
	struct EpollIter
	{
		int num;
		int cur;
		static epoll_event events[EPOLL_MAX_E];
		int operator* ();
		bool readable();
		bool tcp();
		void next() {++cur;}
		bool hasNext() {return cur < num;}
		EpollIter() : num(0), cur(0) {}
	};
	
	struct IP
	{
		sockaddr_in address;
		IP(sockaddr_in sa) : address(sa) {}
		IP(const char * name, unsigned short port);
		IP(unsigned long addr, unsigned short port);
		unsigned short port();
		unsigned long addr();
	};

	int epollCreate();
	void epollWatchRead(int epollfd, int fd);
	void epollWatchWrite(int epollfd, int fd);
	EpollIter epollWait(int epollfd);
	
	bool read(int fd, Buffer & b);
	
	int udpOpen(short port);
};

#endif
