gcc -o green green.c -lpthread
gcc -o test_final test_final.c green.c -lpthread
./test_final

# to run, type:       $ bash -x ./compile.sh
