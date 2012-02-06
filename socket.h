#include <sys/epoll.h>

#define EPOLL_MAX_E 10

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
		void next() {++cur;}
		bool hasNext() {return cur < num;}
		EpollIter() : num(0), cur(0) {}
	};

	int epollCreate();
	void epollWatchRead(int epollfd, int fd);
	void epollWatchWrite(int epollfd, int fd);
	epollIter epollWait(int epollfd);
		
	
	int udpOpen(short port);
};
