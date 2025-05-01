reset

set terminal wxt size 1200,800 background rgb 'black' enhanced font "Arial,10" persist

set border linecolor rgb "white"
set grid linecolor rgb "#555555"
set key textcolor rgb "white"
set tics textcolor rgb "white"

set style line 1 linecolor rgb "#FF0000" linewidth 3
set style line 2 linecolor rgb "#FF00FF" linewidth 3
set style line 3 linecolor rgb "#FFFF00" linewidth 3
set style line 4 linecolor rgb "#00FFFF" linewidth 3

set multiplot title "CherryLimeade9 Rocket Simulation" font "Arial,16" textcolor rgb "white"
set margins 5, 5, 3, 3

set size 0.5, 0.5

set origin 0.0, 0.5
set title "Trajectory" textcolor rgb "white" font "Arial,12"
set xlabel "Horizontal Distance (ft)" textcolor rgb "white" font "Arial,10"
set ylabel "Vertical Distance (ft)" textcolor rgb "white" font "Arial,10"
set grid
set xrange [-150:150]
set yrange [0:1598.45]
plot 'rocket_data.txt' using 2:3 with lines linestyle 1 title "Trajectory"

set origin 0.5, 0.5
set title "Altitude vs. Time" textcolor rgb "white" font "Arial,12"
set xlabel "Time (s)" textcolor rgb "white" font "Arial,10"
set ylabel "Altitude (ft)" textcolor rgb "white" font "Arial,10"
set grid
set xrange [0:26.851]
set yrange [0:1598.45]
plot 'rocket_data.txt' using 1:3 with lines linestyle 2 title "Altitude"

set origin 0.0, 0.0
set title "Mass vs. Time" textcolor rgb "white" font "Arial,12"
set xlabel "Time (s)" textcolor rgb "white" font "Arial,10"
set ylabel "Mass (lb)" textcolor rgb "white" font "Arial,10"
set grid
set xrange [0:26.851]
set yrange [992.079:1212.54]
plot 'rocket_data.txt' using 1:6 with lines linestyle 3 title "Mass"

set origin 0.5, 0.0
set title "Thrust vs. Time" textcolor rgb "white" font "Arial,12"
set xlabel "Time (s)" textcolor rgb "white" font "Arial,10"
set ylabel "Thrust (lbf)" textcolor rgb "white" font "Arial,10"
set grid
set xrange [0:10]
set yrange [0:148374]
plot 'rocket_data.txt' using 1:7 with lines linestyle 4 title "Thrust"

set label 1 "KEY METRICS:" at screen 0.75, 0.42 textcolor rgb "#FFFFFF" font "Arial,11"
set label 2 "Max Q:" at screen 0.75, 0.38 textcolor rgb "#FF9900" font "Arial,10"
set label 3 "3940.11 psf at t=24.4 s" at screen 0.92, 0.38 textcolor rgb "#FFFFFF" font "Arial,10" right

set label 4 "Burnout Alt:" at screen 0.75, 0.35 textcolor rgb "#FF9900" font "Arial,10"
set label 5 "843.557 ft" at screen 0.92, 0.35 textcolor rgb "#FFFFFF" font "Arial,10" right

set label 6 "Burnout Vel:" at screen 0.75, 0.32 textcolor rgb "#FF9900" font "Arial,10"
set label 7 "222.617 ft/s" at screen 0.92, 0.32 textcolor rgb "#FFFFFF" font "Arial,10" right

set label 8 "Apogee:" at screen 0.75, 0.29 textcolor rgb "#FF9900" font "Arial,10"
set label 9 "1453.14 ft at t=14.89 s" at screen 0.92, 0.29 textcolor rgb "#FFFFFF" font "Arial,10" right

unset multiplot
pause -1 "Press any key to close..."
