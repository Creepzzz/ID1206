gcc -c green.c -g
gcc -g test_timer.c -o test_timer.o green.o
gdb test_timer.o

# to run, type:       $ bash -x ./debug.sh
# and in gdb type:    $ run
