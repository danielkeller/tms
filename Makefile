.KEEP_STATE:
SHELL=/bin/bash

CPPFLAGS = -g -Wall

#not including tms.o
OBJ	= controller.o state.o buffer.o socket.o

tms: $(OBJ)

$(OBJ): *.h
