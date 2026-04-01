#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <chrono>
#include <map>

// Include common headers
#include "common/types.h"
#include "common/utils.h"
#include "common/algorithm.h"

// Include test case generator (header + implementations)
#include "testcase_generator/generator.h"
#include "testcase_generator/random.cpp"
#include "testcase_generator/adversarial.cpp"
#include "testcase_generator/falkenauer_parser.cpp"

// Include algorithm implementations from their respective files
#include "algorithms/online/next_fit.cpp"
#include "algorithms/online/first_fit.cpp"
#include "algorithms/online/best_fit.cpp"
#include "algorithms/offline/ffd.cpp"
#include "algorithms/offline/bfd.cpp"
#include "algorithms/optimal/brute_force.cpp"
#include "algorithms/optimal/bitmask_dp.cpp"

// ============================================================================
// Experiment Runner
// ============================================================================

struct ExperimentResult {
    string algorithm;
    int n;
    string test_type;
    int bins_used;
    double waste;
    double runtime_ms;
    double approx_ratio;  // -1 if optimal not computed

    void print_csv(ostream& out) const {
        out << algorithm << ","
            << n << ","
            << test_type << ","
            << bins_used << ","
            << fixed << setprecision(6) << waste << ","
            << fixed << setprecision(4) << runtime_ms << ","
            << fixed << setprecision(4) << approx_ratio << "\n";
    }
};

// Run a single algorithm on a test case and return results
ExperimentResult run_algorithm(BinPackingAlgorithm* algo,
                               const TestCase& test_case,
                               int optimal_bins = -1) {
    ExperimentResult result;
    result.algorithm = algo->name();
    result.n = test_case.items.size();
    result.test_type = test_case.type;

    Timer timer;
    timer.start();
    Packing packing = algo->pack(test_case.items);
    timer.stop();

    result.bins_used = packing.size();
    result.waste = calculate_waste(packing);
    result.runtime_ms = timer.elapsed_ms();

    if (optimal_bins > 0) {
        result.approx_ratio = approximation_ratio(result.bins_used, optimal_bins);
    } else {
        result.approx_ratio = -1.0;
    }

    return result;
}

// Run all experiments (random, adversarial, and Falkenauer benchmarks)
void run_experiments(const string& output_file, const string& benchmark_dir) {
    // Create algorithms
    vector<unique_ptr<BinPackingAlgorithm>> approx_algorithms;
    approx_algorithms.push_back(make_unique<NextFit>());
    approx_algorithms.push_back(make_unique<FirstFit>());
    approx_algorithms.push_back(make_unique<BestFit>());
    approx_algorithms.push_back(make_unique<FFD>());
    approx_algorithms.push_back(make_unique<BFD>());

    unique_ptr<BinPackingAlgorithm> bitmask_dp = make_unique<BitmaskDP>();

    // Results storage
    vector<ExperimentResult> results;

    // Test sizes: small (with exact optimal), larger (no optimal, ratio=-1)
    vector<int> small_sizes = {8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    vector<int> larger_sizes = {25, 30, 40, 50, 75, 100, 150, 200, 300, 500, 750, 1000};

    // Number of samples per (n, generator) combination
    const int NUM_SAMPLES = 100;
    const int BASE_SEED = 42;

    // Helper lambda to create generators with a given seed
    auto create_generators = [](int seed) {
        vector<unique_ptr<TestCaseGenerator>> generators;
        generators.push_back(make_unique<UniformGenerator>(0.1, 0.9, seed));
        generators.push_back(make_unique<NormalGenerator>(0.5, 0.2, 0.05, 0.95, seed));
        generators.push_back(make_unique<OverHalfGenerator>(seed));
        generators.push_back(make_unique<HarmonicGenerator>(seed));
        generators.push_back(make_unique<MixedGenerator>(seed));
        return generators;
    };

    // Parse Falkenauer benchmark files
    cout << "=== Loading Falkenauer Benchmarks ===\n";
    vector<FalkenauerInstance> falkenauer_instances = FalkenauerParser::parse_all(benchmark_dir);
    cout << "Loaded " << falkenauer_instances.size() << " Falkenauer benchmark instances\n\n";

    int total_random_tests = (small_sizes.size() + larger_sizes.size()) * 5 * NUM_SAMPLES;
    int total_tests = total_random_tests + falkenauer_instances.size();
    int test_id = 0;

    cout << "Starting experiments...\n";
    cout << "Total test cases: " << total_tests << "\n";
    cout << "  Random/Adversarial: " << total_random_tests << " (" << NUM_SAMPLES << " samples per config)\n";
    cout << "  Falkenauer Benchmarks: " << falkenauer_instances.size() << "\n\n";

    // ========================================================================
    // Part 1: Small sizes (with optimal solution)
    // ========================================================================
    cout << "=== Small Inputs (with optimal) ===\n";
    for (int n : small_sizes) {
        for (int sample = 0; sample < NUM_SAMPLES; ++sample) {
            int seed = BASE_SEED + sample;
            auto generators = create_generators(seed);
            // n = n + rand_int(-n/10, n/10);  // Add some variability to n
            // if (n > 20) n = 20;  // Cap at 20 for optimal computation

            for (auto& gen : generators) {
                test_id++;
                TestCase test = gen->generate(n);

                if (sample == 0) {
                    cout << "Test " << test_id << "/" << total_tests
                         << ": n=" << n << ", type=" << test.type << "\n";
                }

                // Compute optimal using BitmaskDP
                Packing opt_packing = bitmask_dp->pack(test.items);
                int optimal_bins = opt_packing.size();

                // Run all approximation algorithms
                for (auto& algo : approx_algorithms) {
                    ExperimentResult res = run_algorithm(algo.get(), test, optimal_bins);
                    results.push_back(res);
                }
            }
        }
        cout << "  Completed n=" << n << " (" << NUM_SAMPLES << " samples x 5 generators)\n";
    }

    // ========================================================================
    // Part 2: Larger sizes (no optimal - use -1)
    // ========================================================================
    cout << "\n=== Larger Inputs (no optimal) ===\n";
    for (int n : larger_sizes) {
        for (int sample = 0; sample < NUM_SAMPLES; ++sample) {
            int seed = BASE_SEED + sample;
            auto generators = create_generators(seed);
            // n = n + rand_int(-n/10, n/10);  // Add some variability to n

            for (auto& gen : generators) {
                test_id++;
                TestCase test = gen->generate(n);

                for (auto& algo : approx_algorithms) {
                    ExperimentResult res = run_algorithm(algo.get(), test, -1);
                    results.push_back(res);
                }
            }
        }
        cout << "  Completed n=" << n << " (" << NUM_SAMPLES << " samples x 5 generators)\n";
    }

    // ========================================================================
    // Part 3: Falkenauer Benchmarks (known optimal)
    // ========================================================================
    cout << "\n=== Falkenauer Benchmarks ===\n";
    
    // Separate U and T class for reporting
    int u_count = 0, t_count = 0;
    for (const auto& inst : falkenauer_instances) {
        if (inst.instance_class == "U") u_count++;
        else t_count++;
    }
    cout << "  U-class instances: " << u_count << "\n";
    cout << "  T-class instances: " << t_count << "\n\n";

    for (const auto& instance : falkenauer_instances) {
        test_id++;
        TestCase test = FalkenauerParser::to_test_case(instance);
        
        cout << "Test " << test_id << "/" << total_tests 
             << ": " << instance.name 
             << " (n=" << instance.num_items 
             << ", OPT=" << instance.optimal_bins << ")\n";

        // Run all approximation algorithms with known optimal
        for (auto& algo : approx_algorithms) {
            ExperimentResult res = run_algorithm(algo.get(), test, instance.optimal_bins);
            results.push_back(res);
        }
    }

    // ========================================================================
    // Write all results to CSV
    // ========================================================================
    ofstream csv(output_file);
    csv << "algorithm,n,test_type,bins_used,waste,runtime_ms,approx_ratio\n";
    for (const auto& res : results) {
        res.print_csv(csv);
    }
    csv.close();

    // ========================================================================
    // Print summary statistics
    // ========================================================================
    cout << "\n=== Experiment Complete ===\n";
    cout << "Results saved to: " << output_file << "\n";
    cout << "Total results: " << results.size() << "\n";

    // Falkenauer summary
    map<string, vector<double>> u_ratios, t_ratios;
    for (const auto& res : results) {
        if (res.test_type == "Falkenauer_U") {
            u_ratios[res.algorithm].push_back(res.approx_ratio);
        } else if (res.test_type == "Falkenauer_T") {
            t_ratios[res.algorithm].push_back(res.approx_ratio);
        }
    }

    if (!u_ratios.empty() || !t_ratios.empty()) {
        cout << "\n=== Falkenauer Benchmark Summary ===\n";
        cout << "Algorithm       | U-class  | T-class\n";
        cout << "----------------|----------|--------\n";
        
        vector<string> algo_names = {"NextFit", "FirstFit", "BestFit", "FFD", "BFD"};
        for (const string& algo : algo_names) {
            double u_avg = 0.0, t_avg = 0.0;
            if (!u_ratios[algo].empty()) {
                u_avg = accumulate(u_ratios[algo].begin(), u_ratios[algo].end(), 0.0) / u_ratios[algo].size();
            }
            if (!t_ratios[algo].empty()) {
                t_avg = accumulate(t_ratios[algo].begin(), t_ratios[algo].end(), 0.0) / t_ratios[algo].size();
            }
            cout << setw(15) << left << algo << " | " 
                 << fixed << setprecision(4) << setw(8) << u_avg << " | "
                 << fixed << setprecision(4) << t_avg << "\n";
        }
    }
}

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char* argv[]) {
    string output_file = "results.csv";
    string benchmark_dir = "falkenauer_benchmark";

    cout << "================================================================\n";
    cout << "     Bin Packing Algorithm Experimental Framework              \n";
    cout << "================================================================\n";
    cout << "  Algorithms: NextFit, FirstFit, BestFit, FFD, BFD             \n";
    cout << "  Optimal: BitmaskDP (small N and Falkenauer benchmarks)       \n";
    cout << "  Test Types: Random, Adversarial, Falkenauer (U & T class)    \n";
    cout << "================================================================\n\n";

    // Parse arguments
    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--benchmark-dir" && i + 1 < argc) {
            benchmark_dir = argv[++i];
        } else if (arg == "--help" || arg == "-h") {
            cout << "Usage: runner [OUTPUT_FILE] [options]\n";
            cout << "Options:\n";
            cout << "  OUTPUT_FILE              Output CSV file (default: results.csv)\n";
            cout << "  --benchmark-dir DIR      Falkenauer benchmark directory (default: falkenauer_benchmark)\n";
            cout << "  --help, -h               Show this help message\n";
            return 0;
        } else if (arg[0] != '-') {
            output_file = arg;
        }
    }

    try {
        run_experiments(output_file, benchmark_dir);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
