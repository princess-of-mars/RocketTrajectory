This project simulates the flight of a model rocket using a custom thrust curve and outputs both simulation data and visual plots. The simulation is performed in imperial units (feet, pounds, etc.) and generates key flight metrics and plots using Gnuplot.

## Features

- Reads a thrust curve from a data file (`CherryLimeade9.txt`)
- Simulates 2D rocket flight with drag, wind, and mass loss due to fuel burn
- Outputs time-series data to `rocket_data.txt`
- Automatically generates a Gnuplot script (`plot.gp`) to visualize:
  - Trajectory (x vs y)
  - Altitude vs. Time
  - Mass vs. Time
  - Thrust vs. Time
- Displays key metrics: Max Q, burnout altitude/velocity, and apogee

## Requirements

- C++14 compatible compiler (e.g., MSVC, g++, clang++)
- [Gnuplot](http://www.gnuplot.info/) installed at `C:\Program Files\gnuplot\bin\gnuplot.exe` (or adjust the path in `main.cpp`)
- Input file: `CherryLimeade9.txt` (thrust curve: time [s], thrust [N], space-separated, one pair per line)

## Building

1. Open the project in Visual Studio 2022.
2. Build the solution (default settings are sufficient for C++14).

## Running

1. Ensure `CherryLimeade9.txt` is present in the executable's working directory (typically the `Debug` folder).
2. Run the program. It will:
   - Read the thrust curve
   - Simulate the rocket flight
   - Write results to `rocket_data.txt`
   - Generate and execute `plot.gp` to display plots

## Output Files

- `rocket_data.txt`: Simulation data (columns: time, x, y, vx, vy, mass, thrust)
- `plot.gp`: Gnuplot script for visualization

## Example Thrust Curve (`CherryLimeade9.txt`)
