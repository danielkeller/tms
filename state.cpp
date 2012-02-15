#include "state.h"
#include "buffer.h"
#include "data.h"
#include "controller.h"
#include <time.h>
#include <iostream>
#include <algorithm>
using namespace std;

//for udp, end of data means bad packet, don't wait
//for tcp, end of data means more is (probably) on the way, wait
#define TTEST(x) if (!(x)) {b->reset(); return Wait;}
#define UTEST(x) if (!(x)) {return Err;}
//for watcher, don't return a value
#define WTEST(x) if (!(x)) {return;}

void RequestInfo(Socket::IP addr)
{
	Controller * ctrl = Controller::get();
	ctrl->writeTo(torquePort, addr, INFO_REQ);
	ctrl->writeTo(torquePort, addr, (char)0);
	ctrl->writeTo(torquePort, addr, (int)0);
}

void HeartWatcher::handle(Buffer * b, Socket::IP src)
{
	if (!Data::servs.count(src))
		RequestInfo(src); //request info the first time
	Data::servs[src].heartbeat = time(NULL);
}

void HeartWatcher::handle_al()
{
	for(map<Socket::IP, Data::Server>::iterator it = Data::servs.begin(); it != Data::servs.end();)
		if (time(NULL) - it->second.heartbeat > SRV_MAX_AGE)
			Data::servs.erase(it++);
		else
			++it;
}

void InfoWatcher::handle(Buffer * b, Socket::IP src)
{
	Data::servs[src].info = time(NULL);
	WTEST(b->skip(6));
	WTEST(b->read(Data::servs[src].game));
	WTEST(b->read(Data::servs[src].mission));
	WTEST(b->read(Data::servs[src].game));
	WTEST(b->read(Data::servs[src].maxp));
	WTEST(b->read(Data::servs[src].region));
	WTEST(b->read(Data::servs[src].version));
	WTEST(b->read(Data::servs[src].flags));
	WTEST(b->read(Data::servs[src].bots));
	int cpu;
	WTEST(b->read(cpu));
	Data::servs[src].cpu = cpu; //int -> short
	signed char players;
	WTEST(b->read(players));
	Data::servs[src].players.clear();
	for (int i=1; i<players; ++i)
	{
		int playerid;
		WTEST(b->read(playerid));
		Data::servs[src].players.push_back(playerid);
	}
	//maintain sorted list
	sort(Data::servs[src].players.begin(), Data::servs[src].players.end());
	
}

void InfoWatcher::handle_al()
{
	for(map<Socket::IP, Data::Server>::iterator it = Data::servs.begin(); it != Data::servs.end(); ++it)
		if (time(NULL) - it->second.info > SRV_REFR_AGE)
			RequestInfo(it->first);
}

void ListWatcher::handle(Buffer * b, Socket::IP src)
{
	char page, minp, maxp, flags, maxb;
	short mincpu;
	int key, region, version;
	string game, mission;
	vector<int> buddies;

	WTEST(b->skip(2));
	WTEST(b->read(key));
	WTEST(b->read(page));
	WTEST(b->read(game));
	WTEST(b->read(mission));
	WTEST(b->read(minp));
	WTEST(b->read(maxp));
	WTEST(b->read(region));
	WTEST(b->read(version));
	WTEST(b->read(flags));
	WTEST(b->read(maxb));
	WTEST(b->read(mincpu));
	char bcount;
	WTEST(b->read(bcount));
	for (int i=1; i<bcount; ++i)
	{
		int playerid;
		WTEST(b->read(playerid));
		buddies.push_back(playerid);
	}
	sort(buddies.begin(), buddies.end());
	
	vector<int> intsct;
	
	//use pointers to avoid copying lots of memory
	vector<const Socket::IP*> flt_result;
	for (map<Socket::IP, Data::Server>::iterator it = Data::servs.begin(); it != Data::servs.end(); ++it)
		if ( it->second.game == game && (mission != "Any" && it->second.mission == mission)
//			&& it->second.players.size() >= minp && it->second.players.size() <= maxp //not in stock Torque
			&& (!region || region & it->second.region) && version == it->second.version
			&& (flags & it->second.flags) == flags && it->second.bots <= maxb && it->second.cpu <= mincpu
			&& (!buddies.size() || set_intersection(buddies.begin(), buddies.end(),
				it->second.players.begin(), it->second.players.end(), intsct.begin()) > intsct.begin()) )
			flt_result.push_back(&it->first);

	Controller * ctrl = Controller::get();
	ctrl->writeTo(torquePort, src, LIST_RESP);
	ctrl->writeTo(torquePort, src, (char)0);
	ctrl->writeTo(torquePort, src, key);
	ctrl->writeTo(torquePort, src, (char)0); //packet number
	ctrl->writeTo(torquePort, src, (char)1); //out of
	ctrl->writeTo(torquePort, src, (short)Data::servs.size());
	for (vector<const Socket::IP*>::iterator it = flt_result.begin(); it != flt_result.end(); ++it)
		ctrl->writeTo(torquePort, src, **it);
}
