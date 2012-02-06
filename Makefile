.KEEP_STATE:
SHELL=/bin/bash

CPPFLAGS = -g -Wall

LDFLAGS = $(CPPFLAGS)
CC = $(CXX) #to link properly

SRCS=$(wildcard *.cpp)
OBJS=$(SRCS:.cpp=.o)

tms: $(OBJS)

$(OBJS): *.h

clean:
	rm -f *.o tms
