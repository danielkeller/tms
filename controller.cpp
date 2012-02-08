#include "controller.h"
#include "buffer.h"
#include "state.h"
#include <iostream>

Controller::Controller()
{
	epollfd = Socket::epollCreate();
}

void Controller::loop()
{
	Socket::EpollIter eit;

	while (true)
	{
		//do the acutal wait here.
		eit = Socket::epollWait(epollfd);
		
		for (; eit.hasNext(); eit.next())
		{
			if (eit.readable())
			{
				if (eit.tcp())
					;
				else
				{
					Buffer b;
					while (Socket::read(*eit, b))
					{
						char pid;
						b.peek(pid);
						if (wreads.count(make_pair(*eit, pid)))
							wreads[make_pair(*eit, pid)]->handle(&b);
						else
							cerr << "Got junk packet #" << (int)pid << endl;
						b.clear();
					}
				}
			}
		}
			
	}
}

//watch an open tcp stream for writeable / data
void Controller::watch(int fd, State * s, Type t)
{}

//watch a udp socket for writeable
//if we want data from udp, we will watch permanently
void Controller::watch(int fd, State *s)
{

}

//permanently watch a tcp socket for pending connections
void Controller::watch(int fd, Watcher * s)
{
}

//permanently watch a udp socket for data
void Controller::watch(int fd, char pid, Watcher * s)
{
	if (wreads.count(make_pair(fd, pid)))
		return; //already watching here
	wreads[make_pair(fd, pid)] = s;
	Socket::epollWatchRead(epollfd, fd);
}
