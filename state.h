#include "common.h"
#include <string>
using namespace std;

class Watcher
{
public:
	void handle(Buffer * b);
};

class State
{
public:
	Result handle(Buffer * b);
};

class ListWatcher : public Watcher
{
public:
	void handle(Buffer * b);
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
	Result handle(Buffer * b);
	ListState() {status = WriteResponse;}
};
		
