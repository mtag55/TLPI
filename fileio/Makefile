IDIR=../lib
CC=gcc
CFLAGS=-Wall -I$(IDIR)
VPATH = ../lib

copy: copy.o error_functions.o get_num.o

seek_io: seek_io.o error_functions.o get_num.o

.PHONY: clean
clean:
	rm -f copy seek_io *.o
