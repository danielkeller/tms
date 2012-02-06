#include "common.h"
#include <map>
using namespace std;

class Controller
{
	map<pair<int,char>, State*> reads;
	map<int, State*> writes;
	
	int epollfd;

public:
	Controller();
	void loop(); //program main loop

	enum Type {
		Read,
		Write
	};
	//watch an open tcp stream for writeable / data
	void watch(int fd, State * s, Type t);
	
	//watch a udp socket for writeable
	//if we want data from udp, we will watch permanently
	void watch(int fd, State *s);
	
	//permanently watch a tcp socket for pending connections
	void watch(int fd, Watcher * s, Type t);
	
	//permanently watch a udp socket for data
	void watch(int fd, char pid, Watcher * s, Type t);
};
