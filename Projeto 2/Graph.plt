
set grid
set logscale

# set title "Primeiro Método - Log"
# set title "Segundo Método"
set title "Terceiro Método - Log"
# set title "Comparação dos 3 métodos"
set ylabel "time (s)"
set xlabel "N"

# set xrange[100:1600]

f(x) = a*x + b
fit f(x) "data_part3.dat" using (log($1)):(log($2)) via a,b

set terminal png
set output "part3_log.png"

# plot "data_part1.dat" using 1:2 with linespoints t "Primeiro Método" lt rgb "blue", "data_part2.dat" using 1:2 with linespoints t "Segundo Método" lt rgb "red", "data_part3.dat" using 1:2 with linespoints t "Terceiro Método" lt rgb "green"
plot "data_part3.dat" using 1:2 with linespoints t "" lt rgb "blue", f(x) w l lc rgb "red"