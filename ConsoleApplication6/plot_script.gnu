set terminal wxt size 1400,1000 enhanced font 'Arial,10'
set multiplot layout 4,1 title 'Амплитудная модуляция сигнала (с шумом)' font 'Arial,14'

set yrange [-2:2]
set xlabel 'Время (мс)' font 'Arial,11'
set ylabel 'Амплитуда' font 'Arial,11'
set title 'Модулирующий сигнал (f = 1 кГц)' font 'Arial,12'
set grid
plot 'modulating.dat' with lines linewidth 2 linecolor rgb 'blue' title 'Модулирующий сигнал'

set yrange [-2:2]
set xlabel 'Время (мс)' font 'Arial,11'
set ylabel 'Амплитуда' font 'Arial,11'
set title 'Несущий сигнал (f = 1 МГц)' font 'Arial,12'
set grid
plot 'carrier.dat' with lines linewidth 1 linecolor rgb 'red' title 'Несущий сигнал'

set yrange [-2:2]
set xlabel 'Время (мс)' font 'Arial,11'
set ylabel 'Амплитуда' font 'Arial,11'
set title 'Белый шум (амплитуда = 0)' font 'Arial,12'
set grid
plot 'noise.dat' with lines linewidth 0.5 linecolor rgb 'gray' title 'Белый шум'

set yrange [-2:2]
set xlabel 'Время (мс)' font 'Arial,11'
set ylabel 'Амплитуда' font 'Arial,11'
set title 'АМ сигнал (глубина модуляции = 100%) + белый шум' font 'Arial,12'
set grid
plot 'am_signal.dat' with lines linewidth 1 linecolor rgb 'dark-green' title 'АМ сигнал + шум'

unset multiplot
pause -1 'Нажмите Enter для выхода'
