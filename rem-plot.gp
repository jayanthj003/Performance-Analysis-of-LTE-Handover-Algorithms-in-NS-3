set terminal png
set output 'Graph1/rem_plot.png'
set xlabel "X"
set ylabel "Y"
set cblabel "SINR (dB)"
plot "rem2.out" using ($1):($2):(10*log10($4)) with image
unset output