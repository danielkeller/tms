#include "state.h"
#include "buffer.h"
#include "data.h"
#include "controller.h"
#include <time.h>
#include <iostream>
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
}

void InfoWatcher::handle_al()
{
	for(map<Socket::IP, Data::Server>::iterator it = Data::servs.begin(); it != Data::servs.end(); ++it)
		if (time(NULL) - it->second.info > SRV_REFR_AGE)
			RequestInfo(it->first);
}

void ListWatcher::handle(Buffer * b, Socket::IP src)
{
	cerr << "From: " << src << endl;
	ListState * ls = new ListState();
	if (ls->readReq(b) == Err)
		;
	else
		ls->writeResp(src);
	delete ls;
	//(eventually) don't delete otherwise, will get deleted by controller
}

Result ListState::readReq(Buffer * b)
{
	UTEST(b->skip(2));
	UTEST(b->read(key));
	UTEST(b->read(page));
	UTEST(b->read(game));
	UTEST(b->read(mission));
	UTEST(b->read(minp));
	UTEST(b->read(maxp));
	UTEST(b->read(region));
	UTEST(b->read(version));
	UTEST(b->read(stype));
	UTEST(b->read(maxb));
	UTEST(b->read(mincpu));
	char bcount;
	UTEST(b->read(bcount));
	UTEST(b->skip(4 * bcount));
	
	cerr << game << " " << mission << endl;
	
	return OK;
}

void ListState::writeResp(Socket::IP dst)
{
	Controller * ctrl = Controller::get();
	ctrl->writeTo(torquePort, dst, LIST_RESP);
	ctrl->writeTo(torquePort, dst, (char)0);
	ctrl->writeTo(torquePort, dst, key);
	ctrl->writeTo(torquePort, dst, (char)0); //packet number
	ctrl->writeTo(torquePort, dst, (char)1); //out of
	ctrl->writeTo(torquePort, dst, (short)Data::servs.size());
	for (map<Socket::IP, Data::Server>::iterator it = Data::servs.begin(); it != Data::servs.end(); ++it)
		ctrl->writeTo(torquePort, dst, it->first);
}
	
Result ListState::handle(Buffer * b)
{
	return Done;
}
