#include "common.h"
#include "buffer.h"
#include <map>
#include <set>
using namespace std;

class Controller
{
	map<pair<int,char>, Watcher*> wreads;
	map<int, State*> sreads;
	map<int, map<Socket::IP, Buffer> > writes;
	set<Watcher*> walarms;
	
	static Controller * sng;
	Controller();

public:
	static Controller * get();
	void loop(); //program main loop
	void alarm_notify();

	enum Type {
		Read,
		Write
	};
	
	//perform asynchronous write on fd
	template <typename T>
	void writeTo(int fd, T d);
	template <typename T>
	void writeTo(int fd, Socket::IP addr, T d);
	
	//watch an open tcp stream for data
	void watch(int fd, State* s, Type t);

	//permanently watch a tcp socket for pending connections
	void watch(int fd, Watcher* s);
	
	//permanently watch a udp socket for data
	void watch(int fd, char pid, Watcher* s);
	
	//watch for every-minute alarm
	void awatch(Watcher* w);
};

template <typename T>
void Controller::writeTo(int fd, T d)
{
	writes[fd][Socket::IP::null].write(d);
	Socket::epollWatchWrite(fd);
}

template <typename T>
void Controller::writeTo(int fd, Socket::IP addr, T d)
{
	writes[fd][addr].write(d);
	Socket::epollWatchWrite(fd);
}
