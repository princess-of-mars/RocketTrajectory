set title 'Rocket Trajectory with Fuel Mass Loss and Wind'
set xlabel 'Horizontal Distance (m)'
set ylabel 'Vertical Distance (m)'
set grid
plot 'rocket_data.txt' using 2:3 with lines title 'Trajectory'
pause -1 'Press Enter to continue...'
set title 'Mass vs. Time'
set xlabel 'Time (s)'
set ylabel 'Mass (kg)'
set grid
plot 'rocket_data.txt' using 1:6 with lines title 'Mass'
pause -1 'Press Enter to close...'
