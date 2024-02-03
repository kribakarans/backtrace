# GNU Makefile to build backtrace demo

all: gccbt uwbt gnubt

gccbt:
	gcc -g -no-pie -o gccbt.out src/bt_with_gcc_api.c src/user.c -lbacktrace

gnubt:
	gcc -g -no-pie -o gnubt.out src/bt_with_gnu_api.c src/addr2line.c src/user.c

uwbt:
	gcc -g -no-pie -o uwbt.out  src/bt_with_unwind_api.c src/addr2line.c src/user.c -lunwind -lunwind-generic

clean:
	rm -f *.out
