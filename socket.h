#ifndef _SOCKET_H_
#define _SOCKET_H_

#include <netinet/in.h>
#include <sys/epoll.h>
#include <ostream>
using namespace std;
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
		IP() {}
		IP(sockaddr_in & sa) {set(sa);}
		IP(const char * name, unsigned short port) {set(name, port);}
		IP(unsigned long addr, unsigned short port) {set(addr, port);}
		void set(sockaddr_in & sa);
		void set(const char * name, unsigned short port);
		void set(unsigned long addr, unsigned short port);
		unsigned short port() const;
		unsigned long addr() const;
		unsigned long naddr() const {return address.sin_addr.s_addr;} //torque expects address in big endian
		
		const static IP null;
	};
	
	bool operator<(const IP l, const IP r);

	void epollWatchRead(int fd);
	void epollWatchWrite(int fd);
	void epollUnWatchRead(int fd);
	void epollUnWatchWrite(int fd);
	EpollIter epollWait();
	
	bool read(int fd, Buffer & b);
	bool read(int fd, Buffer & b, IP & src);
	
	//consumes written part of buffer
	//returns true if entire buffer was written succesfully
	bool write(int fd, Buffer & b);
	bool write(int fd, Buffer & b, IP dst);
	
	int udpOpen(short port);
};

ostream& operator<< (ostream& os, const Socket::IP& str);

#endif
