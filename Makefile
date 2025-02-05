all: test bst_runtime_test veb_runtime_test veb_successor_test

CC = g++
OPT= -g -flto -Ofast
CFLAGS = $(OPT) -Wall -march=native
LIBS = -lssl -lcrypto

test: test.cc VEBTree.h VEBTree.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

bst_runtime_test: bst_runtime_test.cc VEBTree.h VEBTree.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

veb_runtime_test: veb_runtime_test.cc VEBTree.h VEBTree.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

veb_successor_test: veb_successor_test.cc VEBTree.h VEBTree.cpp
	$(CC) $(CFLAGS) $^ -o $@ $(LIBS)

clean:
	rm -f test test.o bst_runtime_test bst_runtime_test.o veb_runtime_test veb_runtime_test.o veb_successor_test veb_successor_test.o
