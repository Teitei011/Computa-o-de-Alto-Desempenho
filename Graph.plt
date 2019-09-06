
set grid
# set logscale

# set title "Primeiro Método - Log"
# set title "Segundo Método"
set title "Terceiro Método -Logscale"
set ylabel "time (s)"
set xlabel "N"

set xrange[100:1600]

set terminal png
set output "part3.png"

# plot "data_part1.dat" using 1:2 with linespoints t "Primeiro Método" lt rgb "blue", "data_part2.dat" using 1:2 with linespoints t "Segundo Método" lt rgb "red", "data_part3.dat" using 1:2 with linespoints t "Terceiro Método" lt rgb "green"
plot "data_part3.dat" using 1:2 with linespoints t "" lt rgb "blue"
