#include <iostream>
#include <fstream>
#include <iomanip>
#include <memory>
#include <chrono>

// Include common headers
#include "common/types.h"
#include "common/utils.h"
#include "common/algorithm.h"

// Include test case generator (header + implementations)
#include "testcase_generator/generator.h"
#include "testcase_generator/random.cpp"
#include "testcase_generator/adversarial.cpp"

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

// Run all experiments
void run_experiments(const string& output_file) {
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

    // Test sizes: small (with exact optimal), medium and large (no optimal, ratio=-1)
    // More varied sizes to capture interesting behavior
    vector<int> small_sizes = {8, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20};
    vector<int> medium_sizes = {25, 30, 40, 50, 75, 100};
    vector<int> large_sizes = {150, 200, 300, 500, 750, 1000};

    // Number of samples per (n, generator) combination
    const int NUM_SAMPLES = 100;
    const int BASE_SEED = 42;

    int total_tests = (small_sizes.size() + medium_sizes.size() + large_sizes.size()) * 5 * NUM_SAMPLES;
    int test_id = 0;

    cout << "Starting experiments...\n";
    cout << "Total test cases: " << total_tests << " (" << NUM_SAMPLES << " samples per config)\n\n";

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

    // Run on small sizes (with optimal solution)
    cout << "=== Small Inputs (with optimal) ===\n";
    for (int n : small_sizes) {
        for (int sample = 0; sample < NUM_SAMPLES; ++sample) {
            int seed = BASE_SEED + sample;
            auto generators = create_generators(seed);

            for (auto& gen : generators) {
                test_id++;
                TestCase test = gen->generate(n);

                if (sample == 0) {
                    cout << "Test " << test_id << "/" << total_tests
                         << ": n=" << n << ", type=" << test.type << ", sample=" << (sample + 1) << "\n";
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

    // Run on medium sizes (no optimal - use -1)
    cout << "\n=== Medium Inputs ===\n";
    for (int n : medium_sizes) {
        for (int sample = 0; sample < NUM_SAMPLES; ++sample) {
            int seed = BASE_SEED + sample;
            auto generators = create_generators(seed);

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

    // Run on large sizes (no optimal - use -1)
    cout << "\n=== Large Inputs ===\n";
    for (int n : large_sizes) {
        for (int sample = 0; sample < NUM_SAMPLES; ++sample) {
            int seed = BASE_SEED + sample;
            auto generators = create_generators(seed);

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

    // Write results to CSV
    ofstream csv(output_file);
    csv << "algorithm,n,test_type,bins_used,waste,runtime_ms,approx_ratio\n";
    for (const auto& res : results) {
        res.print_csv(csv);
    }
    csv.close();

    cout << "\n=== Experiment Complete ===\n";
    cout << "Results saved to: " << output_file << "\n";
    cout << "Total results: " << results.size() << "\n";
}

// ============================================================================
// Main Entry Point
// ============================================================================

int main(int argc, char* argv[]) {
    string output_file = "results.csv";

    if (argc > 1) {
        output_file = argv[1];
    }

    cout << "================================================================\n";
    cout << "     Bin Packing Algorithm Experimental Framework              \n";
    cout << "================================================================\n";
    cout << "  Algorithms: NextFit, FirstFit, BestFit, FFD, BFD             \n";
    cout << "  Optimal: BruteForce, BitmaskDP (small N only)                \n";
    cout << "  Test Types: Random (Uniform, Normal), Adversarial            \n";
    cout << "================================================================\n\n";

    try {
        run_experiments(output_file);
    } catch (const exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
