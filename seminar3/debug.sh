gcc -o green green.c -g
gcc -g test_mutex.c -o test_mutex.o green.o
gdb test_mutex.o

# to run, type:       $ bash -x ./debug.sh
# and in gdb type:    (gdb) run
