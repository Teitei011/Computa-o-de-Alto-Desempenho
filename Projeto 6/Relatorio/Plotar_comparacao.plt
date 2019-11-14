set grid
set terminal png
set output "Comparacao_por_tamanho.png"


set logscale x
set title "Comparação com e sem o OpenMp"
set ylabel "Tamanho (Mb)"
set xlabel "Tempo (s)"

set style line 1 \
    linecolor rgb '#0060ad' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5

set style line 2 \
    linecolor rgb 'red' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5



plot "comparacao_de_redes.dat" using 2:1 with linespoints linestyle 2 t "Com OpenMp", "comparacao_de_redes.dat" using 3:1 with linespoints linestyle 1 t "Sem OpenMp"
