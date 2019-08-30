set grid


set xlabel "time (s)"
set ylabel "N"

set terminal png
set output "test_log.png"

plot "data_part1.dat"  t ""
