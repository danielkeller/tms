#include "buffer.h"
#include "stacktrace.h"
#include <string.h>
#include <string>
#include <iostream>

Buffer::Buffer()
{
	pos = end = start = new char[0];
}

Buffer::Buffer(const Buffer& c)
{
	pos = start = new char[c.length()];
	end = start + c.length();
	memcpy(start, c.start, c.length());
}

void Buffer::push(char * d, size_t l)
{
	char * buf = new char[length() + l];
	memcpy(buf, start, length()); //copy current data
	memcpy(buf + length(), d, l); //copy new data
	delete[] start;
	
	pos = buf + (pos - start); //set pos in new buffer
	end = buf + (end - start) + l;
	start = buf;
}
	
void Buffer::pop(size_t l)
{
	if (l >= length())
	{
		//this is bad, but recoverable
		cerr << "Popping more data than exists" << endl;
		print_stacktrace();
		delete[] start;
		pos = end = start = new char[0];
	}
	else
	{
		char * buf = new char[length() - l];
		memcpy(buf + l, start, length()); //copy current data
		delete[] start;
	
		pos = buf + (pos - start) - l; //set pos in new buffer
		end = buf + (end - start) - l;
		start = buf;
	}
}

void Buffer::clear()
{
	delete[] start;
	pos = end = start = new char[0];
}

bool Buffer::skip(size_t n)
{
	if (available() < n)
		return false;
	pos += n;
	return true;
}
