#define _USE_MATH_DEFINES
#include <iostream>
#include <fstream>
#include <cmath>
#include <cstdlib>
#include <vector>
#include <string>

int main() {
    // Simulation parameters
    double m_dry = 500.0;
    double m_fuel = 500.0;
    double m = m_dry + m_fuel;
    double burn_rate = 500.0 / 8.73;
    double g = 9.81;
    double Cd = 0.3;
    double rho = 1.225;
    double A = 0.07;
    double wind_speed = -5.0;
    double theta = 80.0 * M_PI / 180.0;
    double dt = 0.01;
    double t_max = 30.0;

    // Read thrust curve from file
    std::vector<double> thrust_times, thrust_values;
    std::ifstream thrustFile("CherryLimeade9.txt");
    if (!thrustFile.is_open()) {
        std::cerr << "Error: Could not open CherryLimeade9.txt. Ensure it exists in the Debug directory.\n";
        return 1;
    }
    double time, thrust;
    while (thrustFile >> time >> thrust) {
        thrust_times.push_back(time);
        thrust_values.push_back(thrust);
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

    // Open output file
    std::ofstream outFile("rocket_data.txt");
    if (!outFile.is_open()) {
        std::cerr << "Error: Could not open rocket_data.txt for writing.\n";
        return 1;
    }
    outFile << "time x y vx vy mass thrust\n";

    // Initial conditions
    double x = 0.0, y = 0.0;
    double vx = 0.0, vy = 0.0;
    double t = 0.0;

    // Simulation loop
    while (t <= t_max && y >= 0.0) {
        // Interpolate thrust from the thrust curve
        double current_thrust = 0.0;
        if (t <= thrust_times.back()) {
            for (size_t i = 0; i < thrust_times.size() - 1; ++i) {
                if (t >= thrust_times[i] && t <= thrust_times[i + 1]) {
                    double t0 = thrust_times[i], t1 = thrust_times[i + 1];
                    double th0 = thrust_values[i], th1 = thrust_values[i + 1];
                    current_thrust = th0 + (th1 - th0) * (t - t0) / (t1 - t0);
                    break;
                }
            }
        }

        // Debug output
        std::cout << "t: " << t << ", thrust: " << current_thrust << ", y: " << y << ", mass: " << m << "\n";

        outFile << t << " " << x << " " << y << " " << vx << " " << vy << " " << m << " " << current_thrust << "\n";

        double vx_relative = vx - wind_speed;
        double speed = std::sqrt(vx_relative * vx_relative + vy * vy);
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
    }

    outFile.close();
    std::cout << "Simulation complete. Data written to rocket_data.txt\n";

    // Create Gnuplot script file
    std::ofstream plotFile("plot.gp");
    if (!plotFile.is_open()) {
        std::cerr << "Error: Could not create plot.gp for Gnuplot.\n";
        return 1;
    }

    plotFile << "set title 'CherryLimeade9 Rocket Trajectory'\n";
    plotFile << "set xlabel 'Horizontal Distance (m)'\n";
    plotFile << "set ylabel 'Vertical Distance (m)'\n";
    plotFile << "set grid\n";
    plotFile << "plot 'rocket_data.txt' using 2:3 with lines title 'Trajectory'\n";
    plotFile << "pause -1 'Press Enter to continue...'\n";
    plotFile << "set title 'CherryLimeade9 Mass vs. Time'\n";
    plotFile << "set xlabel 'Time (s)'\n";
    plotFile << "set ylabel 'Mass (kg)'\n";
    plotFile << "set grid\n";
    plotFile << "plot 'rocket_data.txt' using 1:6 with lines title 'Mass'\n";
    plotFile << "pause -1 'Press Enter to continue...'\n";
    plotFile << "set title 'CherryLimeade9 Thrust vs. Time'\n";
    plotFile << "set xlabel 'Time (s)'\n";
    plotFile << "set ylabel 'Thrust (N)'\n";
    plotFile << "set grid\n";
    plotFile << "plot 'rocket_data.txt' using 1:7 with lines title 'Thrust'\n";
    plotFile << "pause -1 'Press Enter to close...'\n";
    plotFile.close();

    std::cout << "Generating plots...\n";
    int result = system("\"C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\" plot.gp");
    if (result != 0) {
        std::cerr << "Error: Failed to execute Gnuplot. Ensure Gnuplot is installed at C:\\Program Files\\gnuplot\\bin\\gnuplot.exe\n";
        return 1;
    }

    std::cout << "Plots displayed. Program finished.\n";
    return 0;
}