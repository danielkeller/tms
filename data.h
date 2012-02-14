#include <ctime>
#include <vector>
#include <map>
using namespace std;
#include "common.h"
#include "socket.h"

namespace Data
{
	struct Server
	{
		string game, mission;
		char maxp, flags, bots;
		short cpu;
		int region, version;
		vector<int> players;
		time_t heartbeat;
		time_t info;
	};

	extern map<Socket::IP, Server> servs;
};
