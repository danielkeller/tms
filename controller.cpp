#include "controller.h"
#include "socket.h"

#undef max
#define max(x,y) ((x) > (y) ? (x) : (y))

Controller::Controller()
{
	epollfd = Socket::epollCreate();
	

void Controller::loop()
{
	
	map<pair<int, char>, State*>::iterator it;
	
	EpollIter eit;

	while (true)
	{
		eit = epollWait(epollfd);
		
		for (; eit.hasNext(); eit.next())
		{
			
	}
}

//watch an open tcp stream for more data
void Controller::watcht(int fd, State * s, Type t)
{}

//watch a udp socket for more data
void Controller::watchu(int fd, char pid, State *s, Type t)
{
	if (t == Read)
	{
		if (ureads.count(make_pair(fd, pid)))
			return; //already watching here
		ureads[make_pair(fd, pid)] = s;
		Socket::epollWatchRead(epollfd, fd);
	}
	if (t == Write)
	{
		if (uwrites.count(make_pair(fd, pid)))
			return; //already watching here
		uwrites[make_pair(fd, pid)] = s;
		Socket::epollWatchWrite(epollfd, fd);
	}
}

//permanently watch a tcp socket
void Controller::watcht(int fd, char pid, Watcher * s, Type t)
{
}

//permanently watch a udp socket
void Controller::watchu(int fd, char pid, Watcher * s, Type t)
{
}
