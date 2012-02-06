#include <string>
using namespace std;

class Buffer
{
	char * start; //first byte
	char * pos; //first unread byte
	char * end; //first byte after buffer

public:
	Buffer();
	
	size_t length() {return end-start;}
	size_t available() {return end-pos;}
	void push(char * d, size_t l);
	void pop(size_t l);
	void pop() {pop(pos-start);} //pops all read data
	
	template<typename T>
	bool read(T & d);
	
	bool skip(size_t n);
	
	void reset() {pos = start;}
};

template<typename T>
bool Buffer::read(T & d)
{
	if (available() >= sizeof(T))
	{
		d = *((T*)pos);
		pos += sizeof(T);
		return true;
	}
	else
		return false;
}

template<>
inline bool Buffer::read(string & d)
{
	unsigned char len;
	if (!read(len))
		return false;
	if (available() < len)
		return false;
	d.assign(pos, len);
	pos += len;
	return true;
}
