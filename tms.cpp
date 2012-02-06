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
	
	ctrl.watchu(Socket::udpOpen(TORQUE_MS_PORT), LIST_REQ, new ListWatcher(), Controller::Read);
	
	ctrl.loop();
	
	return 0;
}
