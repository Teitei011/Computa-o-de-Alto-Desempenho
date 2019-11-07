set grid
set terminal png
set output "Eficiencia_por_tamanho.png"


# set logscale x
set title "Ganho de eficiência pelo Tamanho"
set ylabel "%"
set xlabel "Tamanho (Mb)"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5


plot "Diferenca.dat" with linespoints linestyle 1 t ""
