set terminal png
set output "tlb.png"

set title "TLB benchmark, 4KiByte pages, 10 Gi operations"

set key right center

set xlabel "number of pages"
set ylabel "time in s"

#use log scale if we use doubleing of number of pages
set logscale x 2

plot "tlb.dat" u 1:2 w linespoints title "page size 64 bytes", \
"tlb4k.dat" u 1:2 w linespoints title "page size 4K bytes", \
"dummy.dat" u 1:2 w linespoints title "dummy"
