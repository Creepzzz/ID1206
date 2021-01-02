gcc -o green green.c -lpthread
gcc -o test_timer test_timer.c green.c -lpthread
./test_timer

# to run, type:       $ bash -x ./compile.sh
# and in gdb type:    $ run
