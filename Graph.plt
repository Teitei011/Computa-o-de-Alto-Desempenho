
set grid
#set logscale

set title "Multiplicação de matrizes"
set ylabel "time (s)"
set xlabel "N"

set xrange[100:1600]

set terminal png
set output "part1.png"

plot "data_part1.dat" using 1:2 with linespoints t "" lt rgb "blue"
