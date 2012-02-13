#include "common.h"
#include <string>
using namespace std;

//base classes

class Watcher
{
public:
	virtual void handle(Buffer * b, Socket::IP src) = 0;
};

class State
{
public:
	virtual Result handle(Buffer * b) = 0;
};

//derived classes

class HeartWatcher : public Watcher
{
public:
	void handle(Buffer * b, Socket::IP src);
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
		
