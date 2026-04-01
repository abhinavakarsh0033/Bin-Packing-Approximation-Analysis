#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "types.h"

// ============================================================================
// Base Interface for Bin Packing Algorithms
// ============================================================================
// All algorithms must inherit from this class and implement:
// - name(): returns the algorithm name for reporting
// - pack(): takes items and returns a valid packing
// ============================================================================

class BinPackingAlgorithm {
public:
    // Returns the algorithm name (e.g., "FFD", "BestFit", "BruteForce")
    virtual string name() const = 0;

    // Pack items into bins and return the packing
    // Input: vector of items, each in range (0, MaxBinCapacity]
    // Output: Packing where each bin's total <= MaxBinCapacity
    virtual Packing pack(const vector<Item>& items) = 0;

    // Virtual destructor for proper cleanup
    virtual ~BinPackingAlgorithm() {}
};

#endif // ALGORITHM_H
