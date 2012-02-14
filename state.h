#include "common.h"
#include <string>
using namespace std;

//base classes

class Watcher
{
public:
	//handle data recieved on socket
	virtual void handle(Buffer * b, Socket::IP src) = 0;
	//handle timer alarm event
	virtual void handle_al() {}
};

class State
{
public:
	//handle data recived on socket
	virtual Result handle(Buffer * b) = 0;
};

//derived classes

class HeartWatcher : public Watcher
{
public:
	void handle(Buffer * b, Socket::IP src);
	void handle_al();
};

class InfoWatcher : public Watcher
{
public:
	void handle(Buffer * b, Socket::IP src);
	void handle_al();
};

class ListWatcher : public Watcher
{
public:
	void handle(Buffer * b, Socket::IP src);
};
	
class ListState : public State
{
	enum {
		WriteDB,
		ReadDB,
		WriteResponse
	} status;

	char page, minp, maxp, stype, maxb;
	short mincpu;
	int key, region, version;
	string game, mission;
	
public:
	Result readReq(Buffer * b);
	void writeResp(Socket::IP dst);
	Result handle(Buffer * b);
	ListState() {status = WriteResponse;}
};
		
