#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>
#include <direct.h>

int main() {
    // Conversion factors
    const double M_TO_FT = 3.28084;        // Meters to feet
    const double KG_TO_LB = 2.20462;       // Kilograms to pounds
    const double N_TO_LBF = 0.224809;      // Newtons to pounds-force
    const double PA_TO_PSF = 0.0208854;    // Pascals to pounds per square foot
    const double MPS_TO_FPS = 3.28084;     // Meters per second to feet per second

    // Simulation parameters (converted to imperial units)
    double m_dry = 500.0 * KG_TO_LB;       // Dry mass: 500 kg to lb
    double m_fuel = 500.0 * KG_TO_LB;      // Fuel mass: 500 kg to lb
    double m = m_dry + m_fuel;             // Total mass in lb
    double burn_rate = (500.0 / 8.73) * KG_TO_LB; // Burn rate: kg/s to lb/s
    double g = 9.81 * M_TO_FT;             // Gravity: 9.81 m/s^2 to ft/s^2
    double Cd = 0.1;                       // Drag coefficient (unitless)
    double rho = 1.225 * (KG_TO_LB / (M_TO_FT * M_TO_FT * M_TO_FT)); // Air density: kg/m^3 to lb/ft^3
    double A = 0.07 * (M_TO_FT * M_TO_FT); // Area: m^2 to ft^2
    double wind_speed = -5.0 * MPS_TO_FPS; // Wind speed: m/s to ft/s
    double theta = 80.0 * M_PI / 180.0;    // Launch angle in radians
    double dt = 0.01;                      // Time step in seconds
    double t_max = 30.0;                   // Max simulation time in seconds

    // Variables to track key metrics
    double max_q = 0.0, max_q_time = 0.0;
    double burnout_altitude = 0.0, burnout_velocity = 0.0;
    double apogee = 0.0, apogee_time = 0.0;
    double burnout_time = 8.73; // From CherryLimeade9.txt

    // Debug: Print working directory
    char cwd[256];
    _getcwd(cwd, sizeof(cwd));
    std::cout << "Current working directory: " << cwd << "\n";

    // Read thrust curve from file (time in seconds, thrust in Newtons)
    std::vector<double> thrust_times, thrust_values;
    std::ifstream thrustFile("CherryLimeade9.txt");
    if (!thrustFile.is_open()) {
        std::cerr << "Error: Could not open CherryLimeade9.txt. Ensure it exists in the Debug directory.\n";
        return 1;
    }
    double time, thrust;
    while (thrustFile >> time >> thrust) {
        thrust_times.push_back(time);
        thrust_values.push_back(thrust * N_TO_LBF * 20); // Convert to lbf and scale by 20
    }
    thrustFile.close();
    if (thrust_times.empty()) {
        std::cerr << "Error: CherryLimeade9.txt is empty or incorrectly formatted.\n";
        return 1;
    }
    if (thrust_times.size() < 2) {
        std::cerr << "Error: CherryLimeade9.txt must have at least two data points for interpolation.\n";
        return 1;
    }
    std::cout << "Successfully read " << thrust_times.size() << " data points from CherryLimeade9.txt\n";
    std::cout << "First thrust value (at t=" << thrust_times[0] << " s): " << thrust_values[0] << " lbf\n";
    std::cout << "Last thrust value (at t=" << thrust_times.back() << " s): " << thrust_values.back() << " lbf\n";

    // Open output file (store data in imperial units)
    std::ofstream outFile("rocket_data.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open rocket_data.txt for writing.\n";
        return 1;
    }
    outFile << "time x y vx vy mass thrust\n"; // x, y in ft; vx, vy in ft/s; mass in lb; thrust in lbf

    // Initial conditions
    double x = 0.0, y = 0.0; // Position in feet
    double vx = 0.0, vy = 0.0; // Velocity in ft/s
    double t = 0.0;

    // Simulation loop
    int iteration = 0;
    while (t <= t_max && y >= 0.0) {
        // Interpolate thrust from the thrust curve (already in lbf)
        double current_thrust = 0.0;
        if (t <= thrust_times.back()) {
            if (t < thrust_times[0]) {
                current_thrust = thrust_values[0];
            }
            else {
                for (size_t i = 0; i < thrust_times.size() - 1; ++i) {
                    if (t >= thrust_times[i] && t <= thrust_times[i + 1]) {
                        double t0 = thrust_times[i], t1 = thrust_times[i + 1];
                        double th0 = thrust_values[i], th1 = thrust_values[i + 1];
                        current_thrust = th0 + (th1 - th0) * (t - t0) / (t1 - t0);
                        break;
                    }
                }
            }
        }

        // Debug output
        if (iteration < 5 || iteration % 100 == 0) {
            std::cout << "Iteration: " << iteration << ", t: " << t << " s, thrust: " << current_thrust
                << " lbf, x: " << x << " ft, y: " << y << " ft, mass: " << m << " lb, m_fuel: " << m_fuel << " lb\n";
        }

        outFile << t << " " << x << " " << y << " " << vx << " " << vy << " " << m << " " << current_thrust << "\n";

        // Calculate dynamic pressure (q) and track max Q
        double vx_relative = vx - wind_speed;
        double speed = std::sqrt(vx_relative * vx_relative + vy * vy);
        double q = 0.5 * rho * speed * speed; // q in lb/ft^2
        if (q > max_q) {
            max_q = q;
            max_q_time = t;
        }

        // Track burnout metrics
        if (t >= burnout_time && burnout_altitude == 0.0) {
            burnout_altitude = y;
            burnout_velocity = speed;
        }

        // Track apogee
        if (y > apogee) {
            apogee = y;
            apogee_time = t;
        }

        double drag = 0.5 * rho * Cd * A * speed * speed;
        double drag_x = (speed > 0) ? -drag * vx_relative / speed : 0.0;
        double drag_y = (speed > 0) ? -drag * vy / speed : 0.0;

        double Fx = current_thrust * std::cos(theta) + drag_x;
        double Fy = current_thrust * std::sin(theta) - m * g + drag_y;

        double ax = Fx / m;
        double ay = Fy / m;

        vx += ax * dt;
        vy += ay * dt;

        x += vx * dt;
        y += vy * dt;

        if (current_thrust > 0 && m_fuel > 0) {
            m_fuel -= burn_rate * dt;
            if (m_fuel < 0) m_fuel = 0;
            m = m_dry + m_fuel;
        }

        t += dt;
        iteration++;
    }

    outFile.close();
    std::cout << "Simulation complete. Data written to rocket_data.txt\n";
    std::cout << "Final t: " << t << " s, Final y: " << y << " ft\n";
    std::cout << "Max Q: " << max_q << " psf at t=" << max_q_time << " s\n";
    std::cout << "Burnout Altitude: " << burnout_altitude << " ft, Velocity: " << burnout_velocity << " ft/s\n";
    std::cout << "Apogee: " << apogee << " ft at t=" << apogee_time << " s\n";

    // Create Gnuplot script file with improved multiplot layout
    std::ofstream plotFile("plot.gp");
    if (!plotFile.is_open()) {
        std::cerr << "Error: Could not create plot.gp for Gnuplot.\n";
        return 1;
    }

    // Reset all previous settings
    plotFile << "reset\n\n";

    // Set terminal with appropriate size and black background
    plotFile << "set terminal wxt size 1200,800 background rgb 'black' enhanced font \"Arial,10\" persist\n\n";

    // Global styling
    plotFile << "set border linecolor rgb \"white\"\n";
    plotFile << "set grid linecolor rgb \"#555555\"\n";
    plotFile << "set key textcolor rgb \"white\"\n";
    plotFile << "set tics textcolor rgb \"white\"\n\n";

    // Define bright line styles
    plotFile << "set style line 1 linecolor rgb \"#FF0000\" linewidth 3\n";  // Red for trajectory
    plotFile << "set style line 2 linecolor rgb \"#FF00FF\" linewidth 3\n";  // Magenta for altitude
    plotFile << "set style line 3 linecolor rgb \"#FFFF00\" linewidth 3\n";  // Yellow for mass
    plotFile << "set style line 4 linecolor rgb \"#00FFFF\" linewidth 3\n\n";  // Cyan for thrust

    // Set up multiplot with clear margins and spacing
    plotFile << "set multiplot title \"CherryLimeade9 Rocket Simulation\" font \"Arial,16\" textcolor rgb \"white\"\n";
    plotFile << "set margins 5, 5, 3, 3\n\n";  // left, right, bottom, top

    // Define layout for 2x2 grid
    plotFile << "set size 0.5, 0.5\n\n";

    // Plot 1: Trajectory (top-left)
    plotFile << "set origin 0.0, 0.5\n";
    plotFile << "set title \"Trajectory\" textcolor rgb \"white\" font \"Arial,12\"\n";
    plotFile << "set xlabel \"Horizontal Distance (ft)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set ylabel \"Vertical Distance (ft)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set grid\n";
    plotFile << "set xrange [-150:150]\n";
    plotFile << "set yrange [0:" << (apogee > 0 ? apogee * 1.1 : 1600) << "]\n";
    plotFile << "plot 'rocket_data.txt' using 2:3 with lines linestyle 1 title \"Trajectory\"\n\n";

    // Plot 2: Altitude vs Time (top-right)
    plotFile << "set origin 0.5, 0.5\n";
    plotFile << "set title \"Altitude vs. Time\" textcolor rgb \"white\" font \"Arial,12\"\n";
    plotFile << "set xlabel \"Time (s)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set ylabel \"Altitude (ft)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set grid\n";
    plotFile << "set xrange [0:" << (t > 0 ? t * 1.1 : 25) << "]\n";
    plotFile << "set yrange [0:" << (apogee > 0 ? apogee * 1.1 : 1600) << "]\n";
    plotFile << "plot 'rocket_data.txt' using 1:3 with lines linestyle 2 title \"Altitude\"\n\n";

    // Plot 3: Mass vs Time (bottom-left)
    plotFile << "set origin 0.0, 0.0\n";
    plotFile << "set title \"Mass vs. Time\" textcolor rgb \"white\" font \"Arial,12\"\n";
    plotFile << "set xlabel \"Time (s)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set ylabel \"Mass (lb)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set grid\n";
    plotFile << "set xrange [0:" << (t > 0 ? t * 1.1 : 25) << "]\n";
    plotFile << "set yrange [" << m_dry * 0.9 << ":" << (m_dry + m_fuel) * 1.1 << "]\n";
    plotFile << "plot 'rocket_data.txt' using 1:6 with lines linestyle 3 title \"Mass\"\n\n";

    // Plot 4: Thrust vs Time (bottom-right)
    plotFile << "set origin 0.5, 0.0\n";
    plotFile << "set title \"Thrust vs. Time\" textcolor rgb \"white\" font \"Arial,12\"\n";
    plotFile << "set xlabel \"Time (s)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set ylabel \"Thrust (lbf)\" textcolor rgb \"white\" font \"Arial,10\"\n";
    plotFile << "set grid\n";
    plotFile << "set xrange [0:10]\n";
    plotFile << "set yrange [0:" << 30000 * N_TO_LBF * 20 * 1.1 << "]\n";
    plotFile << "plot 'rocket_data.txt' using 1:7 with lines linestyle 4 title \"Thrust\"\n\n";

    // Add key metrics box
    plotFile << "set label 1 \"KEY METRICS:\" at screen 0.75, 0.42 textcolor rgb \"#FFFFFF\" font \"Arial,11\"\n";
    plotFile << "set label 2 \"Max Q:\" at screen 0.75, 0.38 textcolor rgb \"#FF9900\" font \"Arial,10\"\n";
    plotFile << "set label 3 \"" << max_q << " psf at t=" << max_q_time << " s\" at screen 0.92, 0.38 textcolor rgb \"#FFFFFF\" font \"Arial,10\" right\n\n";

    plotFile << "set label 4 \"Burnout Alt:\" at screen 0.75, 0.35 textcolor rgb \"#FF9900\" font \"Arial,10\"\n";
    plotFile << "set label 5 \"" << burnout_altitude << " ft\" at screen 0.92, 0.35 textcolor rgb \"#FFFFFF\" font \"Arial,10\" right\n\n";

    plotFile << "set label 6 \"Burnout Vel:\" at screen 0.75, 0.32 textcolor rgb \"#FF9900\" font \"Arial,10\"\n";
    plotFile << "set label 7 \"" << burnout_velocity << " ft/s\" at screen 0.92, 0.32 textcolor rgb \"#FFFFFF\" font \"Arial,10\" right\n\n";

    plotFile << "set label 8 \"Apogee:\" at screen 0.75, 0.29 textcolor rgb \"#FF9900\" font \"Arial,10\"\n";
    plotFile << "set label 9 \"" << apogee << " ft at t=" << apogee_time << " s\" at screen 0.92, 0.29 textcolor rgb \"#FFFFFF\" font \"Arial,10\" right\n\n";

    plotFile << "unset multiplot\n";
    plotFile << "pause -1 \"Press any key to close...\"\n";
    plotFile.close();
   ;

    std::cout << "Generating plots...\n";
    int result = system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" plot.gp");
    if (result != 0) {
        std::cerr << "Error: Failed to execute Gnuplot at C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\n";
        std::cerr << "Please ensure Gnuplot is installed correctly or adjust the path.\n";
        return 1;
    }

    std::cout << "Plots displayed. Program finished.\n";
    return 0;
}