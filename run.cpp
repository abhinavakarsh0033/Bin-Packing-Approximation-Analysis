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

int main()
{
    vector<double> custom_test = {11.0/16, 6.0/16, 6.0/16, 3.0/16, 2.0/16, 2.0/16, 2.0/16};
    // run on all algorithms
    vector<unique_ptr<BinPackingAlgorithm>> algorithms;
    algorithms.push_back(make_unique<NextFit>());
    algorithms.push_back(make_unique<FirstFit>());
    algorithms.push_back(make_unique<BestFit>());
    algorithms.push_back(make_unique<FFD>());
    algorithms.push_back(make_unique<BFD>());
    unique_ptr<BinPackingAlgorithm> bitmask_dp = make_unique<BitmaskDP>();
    for (auto& algo : algorithms) {
        Packing packing = algo->pack(custom_test);
        cout << algo->name() << " used " << packing.size() << " bins.\n";
    }
    Packing optimal_packing = bitmask_dp->pack(custom_test);
    cout << "Optimal (BitmaskDP) used " << optimal_packing.size() << " bins.\n";
}