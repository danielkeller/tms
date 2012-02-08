#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <iostream>
using namespace std;

#include "socket.h"
//to get def of Socket::IP for read + write

class Buffer
{
	char * start; //first byte
	char * pos; //first unread byte
	char * end; //first byte after buffer

public:
	Buffer();
	~Buffer() {delete[] start;}
	
	size_t length() {return end-start;}
	size_t available() {return end-pos;}
	void push(char * d, size_t l);
	void pop(size_t l);
	void pop() {pop(pos-start);} //pops all read data
	void clear(); //pops all data
	
	template<typename T>
	bool read(T & d);
	
	template<typename T>
	bool peek(T & d);
	
	template<typename T>
	void write(T & d);
	
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
//		cerr << (int)d << endl;
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
//	cerr << d << endl;
	return true;
}

template<>
inline bool Buffer::read(Socket::IP & d)
{
	unsigned long addr;
	unsigned short port;
	if (!read(addr) || !read(port))
		return false;
	d = Socket::IP(addr, port);
	return true;
}

template<typename T>
bool Buffer::peek(T & d)
{
	bool ret = read(d);
	reset();
	return ret;
}

template<typename T>
void Buffer::write(T & d)
{
	push((char*)&d, sizeof(T));
}

template<>
inline void Buffer::write(string & d)
{
	char len = d.length();
	push(&len, sizeof(len));
	push((char*)d.c_str(), len);
}

template<>
inline void Buffer::write(Socket::IP & d)
{
	unsigned long addr = d.addr();
	unsigned short port = d.port();
	write(addr);
	write(port);
}

#endif
