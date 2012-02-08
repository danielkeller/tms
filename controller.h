#include "common.h"
#include <map>
using namespace std;

class Controller
{
	map<pair<int,char>, Watcher*> wreads;
	map<int, Watcher*> wwrites;
	
	int epollfd;

public:
	Controller();
	void loop(); //program main loop

	enum Type {
		Read,
		Write
	};
	
	//watch an open tcp stream for writeable / data
	void watch(int fd, State* s, Type t);
	
	//watch a udp socket for writeable
	//if we want data from udp, we will watch permanently
	//unless we add address filter (which i don't think is needed)
	void watch(int fd, State* s);

	//permanently watch a tcp socket for pending connections
	void watch(int fd, Watcher* s);
	
	//permanently watch a udp socket for data
	void watch(int fd, char pid, Watcher* s);
};
