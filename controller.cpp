#include "controller.h"
#include "buffer.h"
#include "state.h"
#include <iostream>

Controller * Controller::sng = 0;

Controller::Controller()
{
	Socket::epollCreate();
}

Controller * Controller::get()
{
	if (!sng)
		sng = new Controller();
	return sng;
}

void Controller::loop()
{
	Socket::EpollIter eit;
	Socket::IP src;

	while (true)
	{
		//do the acutal wait here.
		eit = Socket::epollWait();
		
		for (; eit.hasNext(); eit.next())
		{
			if (eit.readable())
			{
				if (eit.tcp())
					;
				else
				{
					Buffer b;
					while (Socket::read(*eit, b, src))
					{
						char pid;
						b.peek(pid);
						if (wreads.count(make_pair(*eit, pid)))
							wreads[make_pair(*eit, pid)]->handle(&b, src);
						else
							cerr << "Got junk packet #" << (int)pid << endl;
						b.clear();
					}
				}
			}
			else //write
			{
				for(map<Socket::IP, Buffer>::iterator it = writes[*eit].begin();
					it != writes[*eit].end();)
				{
					//wtf??
					if(//it->first == Socket::IP::null //sending on tcp?
						//? Socket::write(*eit, it->second) //no addr needed
						/*:*/ Socket::write(*eit, it->second, it->first)) //addr needed
					{
						//entire buffer was written, cool
						writes[*eit].erase(it++); //clean up. note post increment.
						Socket::epollUnWatchWrite(*eit);
					}
					else
						++it;
				}
			}
		}
	}
}

//watch an open tcp stream for data
void Controller::watch(int fd, State * s, Type t)
{}

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
	Socket::epollWatchRead(fd);
}
