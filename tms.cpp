#include <iostream>
#include "controller.h"
#include "state.h"
#include "socket.h"
#include <csignal>

using namespace std;

int main()
{
	signal(SIGPIPE, SIG_IGN);

	Controller ctrl;
	
	ListWatcher lw;
	ctrl.watch(Socket::udpOpen(TORQUE_MS_PORT), LIST_REQ, &lw);
	
	ctrl.loop();
	
	return 0;
}
