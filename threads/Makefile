IDIR=../lib
CC=gcc
CFLAGS=-Wall -I$(IDIR) 
VPATH=../lib
LDFLAGS=-pthread

all: simple_thread self_join_thread thread_incr_mutex thread_multijoin

thread_multijoin: thread_multijoin.o error_functions.o get_num.o

simple_thread: simple_thread.o error_functions.o get_num.o

self_join_thread: self_join_thread.o error_functions.o get_num.o

thread_incr_mutex: thread_incr_mutex.o error_functions.o get_num.o

.Phony: clean
clean:
	rm -f thread_multijoin thread_incr_mutex self_join_thread simple_thread *.o
