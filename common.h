#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>

class Buffer;
class Watcher;
class State;
namespace Socket
{
	class IP;
};

//maybe this should be in a namespace?
enum Result {
	Err, //error condition, delete this state
	Wait, //not enough data, keep state & keep watch
	OK, //keep state, delete watch
	Done //no error, delete state
};

//fd of udp port torque talks to
extern int torquePort;

//ports
#define TORQUE_MS_PORT 28002

//packet types
#define LIST_REQ ((char)6)
#define LIST_RESP ((char)8)
#define INFO_REQ ((char)10)
#define INFO_RESP ((char)12)
#define HEARTBEAT ((char)22)

//other
#define EPOLL_MAX_E 10 //max events returned by epoll
#define SERV_PER_LIST 32 //servers sent in one list packet
#define CLOCK_RATE 60 //clock period in seconds
#define SRV_MAX_AGE 60*5 //time before server is removed

#endif
