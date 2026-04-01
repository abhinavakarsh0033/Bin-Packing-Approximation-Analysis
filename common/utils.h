#ifndef UTILS_H
#define UTILS_H

#include <chrono>
#include <cmath>
#include <stdexcept>
#include "types.h"

// ============================================================================
// Timer Utilities
// ============================================================================

// High-resolution timer for measuring algorithm runtime
class Timer {
private:
    std::chrono::high_resolution_clock::time_point start_time;
    std::chrono::high_resolution_clock::time_point end_time;
    bool running;

public:
    Timer() : running(false) {}

    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }

    void stop() {
        end_time = std::chrono::high_resolution_clock::now();
        running = false;
    }

    double elapsed_ms() const {
        auto end = running ? std::chrono::high_resolution_clock::now() : end_time;
        return std::chrono::duration<double, std::milli>(end - start_time).count();
    }
};

// ============================================================================
// Waste Calculation
// ============================================================================

// Calculate total waste in a packing (sum of unused space in all bins)
inline double calculate_waste(const Packing& packing) {
    double total_waste = 0.0;
    for (const Bin& bin : packing) {
        double bin_sum = std::accumulate(bin.begin(), bin.end(), 0.0);
        total_waste += (MaxBinCapacity - bin_sum);
    }
    return total_waste;
}

// ============================================================================
// Approximation Ratio Calculation
// ============================================================================

// Calculate approximation ratio (algorithm_bins / optimal_bins)
inline double approximation_ratio(int algo_bins, int optimal_bins) {
    if (optimal_bins == 0) return 1.0;
    return static_cast<double>(algo_bins) / optimal_bins;
}

// Helper function to generate random integers in a range [min, max]
inline int rand_int(int min, int max) {
    return rand() % (max - min + 1) + min;
}

#endif // UTILS_H
