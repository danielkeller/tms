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

void HeartWatcher::handle(Buffer * b, Socket::IP src)
{
	Data::servs[src].heartbeat = time(NULL);
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
