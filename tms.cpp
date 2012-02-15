#include <iostream>
#include "controller.h"
#include "state.h"
#include "socket.h"
#include <csignal>
using namespace std;

int torquePort;

int main()
{
	signal(SIGPIPE, SIG_IGN);

	Controller * ctrl = Controller::get();
	
	torquePort = Socket::udpOpen(TORQUE_MS_PORT);
	
	ListWatcher lw;
	ctrl->watch(torquePort, LIST_REQ, &lw);
	
	HeartWatcher hw;
	ctrl->watch(torquePort, HEARTBEAT, &hw);
	ctrl->awatch(&hw);
	
	InfoWatcher iw;
	ctrl->watch(torquePort, INFO_RESP, &iw);
	ctrl->awatch(&iw);
	
	//TODO: REST server
	
	ctrl->loop();
	
	return 0;
}
