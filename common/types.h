#ifndef TYPES_H
#define TYPES_H

#include <vector>
#include <string>
#include <numeric>
#include <set>
#include <algorithm>
#include <cmath>
#include <stdexcept>

using namespace std;

// ============================================================================
// Core Type Definitions for Bin Packing
// ============================================================================

// Item: a value in (0, 1] representing the size relative to bin capacity
using Item = double;

// Bin: a collection of items placed in a single bin
using Bin = std::vector<Item>;

// Packing: the complete solution - a collection of bins
using Packing = std::vector<Bin>;

// Maximum capacity of a bin (normalized to 1.0)
const Item MaxBinCapacity = 1.0;

// Maximum number of items for optimal algorithms
const int MAX_OPTIMAL_N = 20;

#endif // TYPES_H
