#include "state.h"
#include "buffer.h"
#include <iostream>
using namespace std;

//for udp, end of data means bad packet, don't wait
//for tcp, end of data means more is (probably) on the way, wait
#define TTEST(x) if (!(x)) {b->reset(); return Wait;}
#define UTEST(x) if (!(x)) {b->reset(); return Err;}

void ListWatcher::handle(Buffer * b)
{
	ListState * ls = new ListState();
	if (ls->readReq(b) == Err)
		delete ls;
	delete ls; //temporary
}

Result ListState::readReq(Buffer * b)
{
	UTEST(b->skip(2));
	UTEST(b->read(key));
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
	status = WriteResponse;
	//queue response
	
	return Done;
}
	
Result ListState::handle(Buffer * b)
{
	
	return Done;
}
