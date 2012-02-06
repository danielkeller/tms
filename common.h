#ifndef _COMMON_H_
#define _COMMON_H_

#include <sys/types.h>

class Buffer;
class Watcher;
class State;
enum Result {
	Err, //error condition, delete this state
	Wait, //not enough data, keep state & keep watch
	Done //keep state, delete watch
};

//ports
#define TORQUE_MS_PORT 28002

//packet types
#define LIST_REQ 6
#define LIST_RESP 8
#define INFO_REQ 10
#define INFO_RESP 12
#define HEARTBEAT 22

#endif
