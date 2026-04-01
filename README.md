# Bin Packing Algorithm Approximation Analysis

An experimental framework for analyzing and comparing bin packing algorithms, including both approximation algorithms and optimal solvers for small inputs.

## Problem

Given $n$ items with sizes $a_1, a_2, \ldots, a_n \in (0, 1]$, find a packing in unit-sized bins that minimizes the number of bins used.

## Features

- **Multiple Algorithms**: Implements 7 bin packing algorithms
- **Test Case Generation**: Random and adversarial input generators
- **Optimal Solutions**: Computes optimal for small inputs (n ≤ 20)
- **Comprehensive Metrics**: Bins used, waste, runtime, approximation ratio
- **Visualization**: Python plotting for analysis

## Algorithms Implemented

### Online Algorithms
| Algorithm | Time | Space | Approx. Ratio |
|-----------|------|-------|---------------|
| Next Fit | O(n) | O(1) | 2 · OPT |
| First Fit | O(n²) | O(n) | 1.7 · OPT + 0.7 |
| Best Fit | O(n log n) | O(n) | 1.7 · OPT + 0.7 |

### Offline Algorithms
| Algorithm | Time | Space | Approx. Ratio |
|-----------|------|-------|---------------|
| First Fit Decreasing (FFD) | O(n²) | O(n) | 11/9 · OPT + 6/9 ≈ 1.22 · OPT |
| Best Fit Decreasing (BFD) | O(n log n) | O(n) | 11/9 · OPT + 6/9 ≈ 1.22 · OPT |

### Optimal Algorithms (Small N only)
| Algorithm | Time | Space | Notes |
|-----------|------|-------|-------|
| Brute Force | O(n! · 2ⁿ) | O(n) | With pruning, practical for n ≤ 15 |
| Bitmask DP | O(n · 2ⁿ) | O(2ⁿ) | Practical for n ≤ 20 |

## Project Structure

```
Bin-Packing-Approximation-Analysis/
├── runner.cpp                      # Main experiment runner
├── Makefile                        # Build system
├── common/
│   ├── types.h                     # Core type definitions
│   ├── algorithm.h                 # Base algorithm interface
│   └── utils.h                     # Utilities (timer, validation, etc.)
├── algorithms/
│   ├── online/
│   │   ├── next_fit.cpp            # Next Fit algorithm
│   │   ├── first_fit.cpp           # First Fit algorithm
│   │   └── best_fit.cpp            # Best Fit algorithm
│   ├── offline/
│   │   ├── ffd.cpp                 # First Fit Decreasing
│   │   └── bfd.cpp                 # Best Fit Decreasing
│   └── optimal/
│       ├── brute_force.cpp         # Backtracking with pruning
│       └── bitmask_dp.cpp          # Dynamic programming over subsets
├── testcase_generator/
│   ├── generator.h                 # Unified generator API
│   ├── random.cpp                  # Random distributions
│   └── adversarial.cpp             # Worst-case inputs
└── analysis/
    └── plot.py                     # Visualization script
```

## Building and Running

### Prerequisites
- C++17 compatible compiler (g++, clang++)
- Python 3 with matplotlib, pandas, numpy

### Quick Start

```bash
# Build and run everything
make all

# Or step by step:
make build    # Compile runner
make run      # Run experiments (outputs results.csv)
make plot     # Generate plots (outputs to plots/)
```

### Manual Build

```bash
# Compile
g++ -std=c++17 -O2 -I. -o runner runner.cpp

# Run
./runner results.csv

# Plot
python analysis/plot.py results.csv plots
```

## Output

### CSV Results
The runner outputs `results.csv` with columns:
- `algorithm`: Algorithm name
- `n`: Number of items
- `test_type`: Type of test case
- `bins_used`: Number of bins used
- `waste`: Total unused capacity
- `runtime_ms`: Execution time in milliseconds
- `approx_ratio`: Ratio to optimal (for small inputs)

### Generated Plots
- `bins_vs_n.png`: Bins used vs input size
- `runtime_vs_n.png`: Runtime scaling (log scale)
- `waste_vs_n.png`: Waste distribution
- `approx_ratio.png`: Approximation ratio (small inputs)
- `algorithm_comparison.png`: Bar chart comparison
- `heatmap_approx_ratio.png`: Ratio heatmap by test type
- `summary.txt`: Statistical summary

## Test Case Types

### Random
- **Uniform**: Items uniformly distributed in [0.1, 0.9]
- **Normal**: Gaussian distribution centered at 0.5
- **Exponential**: Biased towards smaller items

### Adversarial
- **Over Half**: Items slightly > 0.5 (each needs own bin)
- **Under Half**: Items slightly < 0.5 (pairs can combine)
- **Harmonic**: 1/2, 1/3, 1/4, ... (classic worst case)
- **Next Fit Worst**: Alternating large/small items
- **Mixed**: Combination of patterns

## Expected Insights

Running the experiments should reveal:

1. **FFD/BFD ≈ 1.22 OPT**: Offline algorithms achieve the theoretical bound
2. **NextFit ≈ 2.0 OPT**: Simple but worst approximation ratio
3. **FirstFit ≈ BestFit**: Similar performance on most inputs
4. **Runtime Scaling**: BestFit/BFD faster than FirstFit/FFD for large inputs
5. **Adversarial Impact**: Specific inputs expose algorithm weaknesses

## Extending the Framework

### Adding a New Algorithm

1. Create a new `.cpp` file in the appropriate directory
2. Inherit from `BinPackingAlgorithm`:

```cpp
#include "../../common/algorithm.h"

class MyAlgorithm : public BinPackingAlgorithm {
public:
    string name() const override {
        return "MyAlgorithm";
    }

    Packing pack(const vector<Item>& items) override {
        Packing result;
        // Your implementation here
        return result;
    }
};
```

3. Add to `runner.cpp` algorithm list

### Adding Test Cases

Add methods to `TestCaseGenerator` in `generator.h` or extend `RandomGenerator`/`AdversarialGenerator`.