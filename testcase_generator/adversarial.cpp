#include "generator.h"

// ============================================================================
// Adversarial Test Case Generators
// ============================================================================
// Concrete implementations of TestCaseGenerator for adversarial inputs.
// These expose worst-case behavior in bin packing algorithms.
// ============================================================================

// Items slightly larger than 0.5 - each needs its own bin
class OverHalfGenerator : public TestCaseGenerator {
private:
    double epsilon;
    int seed;  // Unused but kept for API consistency

public:
    OverHalfGenerator(int s = 0, double eps = 0.01) : epsilon(eps), seed(s) {}

    string name() const override {
        return "OverHalf";
    }

    TestCase generate(int n) override {
        vector<Item> items(n, 0.5 + epsilon);
        return TestCase(items, "over_half_n" + std::to_string(n),
                       "adversarial_over_half", seed);
    }
};

// Items slightly under 0.5 - pairs can fit together
class UnderHalfGenerator : public TestCaseGenerator {
private:
    double epsilon;

public:
    UnderHalfGenerator(double eps = 0.01) : epsilon(eps) {}

    string name() const override {
        return "UnderHalf";
    }

    TestCase generate(int n) override {
        vector<Item> items(n, 0.5 - epsilon);
        return TestCase(items, "under_half_n" + std::to_string(n),
                       "adversarial_under_half", 0);
    }
};

// Harmonic sequence: 1/2, 1/3, 1/4, ...
class HarmonicGenerator : public TestCaseGenerator {
private:
    int seed;  // Unused but kept for API consistency

public:
    HarmonicGenerator(int s = 0) : seed(s) {}

    string name() const override {
        return "Harmonic";
    }

    TestCase generate(int n) override {
        vector<Item> items(n);
        for (int i = 0; i < n; ++i) {
            items[i] = 1.0 / (i + 2);
        }
        return TestCase(items, "harmonic_n" + std::to_string(n),
                       "adversarial_harmonic", seed);
    }
};

// Worst case for Next Fit: alternating large and small
class NextFitWorstGenerator : public TestCaseGenerator {
public:
    string name() const override {
        return "NextFitWorst";
    }

    TestCase generate(int n) override {
        vector<Item> items;
        for (int i = 0; i < n / 2; ++i) {
            items.push_back(0.51);
            items.push_back(0.49);
        }
        if (n % 2 == 1) items.push_back(0.51);
        return TestCase(items, "next_fit_worst_n" + std::to_string(n),
                       "adversarial_next_fit", 0);
    }
};

// FFD worst case approximation
class FFDWorstGenerator : public TestCaseGenerator {
public:
    string name() const override {
        return "FFDWorst";
    }

    TestCase generate(int n) override {
        int m = n / 3;
        vector<Item> items;
        for (int i = 0; i < m; ++i) {
            items.push_back(0.25 + 0.001);
        }
        for (int i = 0; i < m; ++i) {
            items.push_back(0.25 + 0.002);
        }
        for (int i = 0; i < m; ++i) {
            items.push_back(0.5 - 0.003);
        }
        return TestCase(items, "ffd_worst_n" + std::to_string(n),
                       "adversarial_ffd", 0);
    }
};

// Mixed sizes: large, medium, small
class MixedGenerator : public TestCaseGenerator {
private:
    int seed;  // Unused but kept for API consistency

public:
    MixedGenerator(int s = 0) : seed(s) {}

    string name() const override {
        return "Mixed";
    }

    TestCase generate(int n) override {
        vector<Item> items;
        for (int i = 0; i < n; ++i) {
            switch (i % 3) {
                case 0: items.push_back(0.6); break;
                case 1: items.push_back(0.3); break;
                case 2: items.push_back(0.1); break;
            }
        }
        return TestCase(items, "mixed_n" + std::to_string(n),
                       "adversarial_mixed", seed);
    }
};

// Decreasing sequence from 0.9 to 0.1
class DecreasingGenerator : public TestCaseGenerator {
public:
    string name() const override {
        return "Decreasing";
    }

    TestCase generate(int n) override {
        vector<Item> items(n);
        for (int i = 0; i < n; ++i) {
            items[i] = (n == 1) ? 0.5 : 0.9 - (0.8 * i) / (n - 1);
        }
        return TestCase(items, "decreasing_n" + std::to_string(n),
                       "adversarial_decreasing", 0);
    }
};

// Increasing sequence from 0.1 to 0.9
class IncreasingGenerator : public TestCaseGenerator {
public:
    string name() const override {
        return "Increasing";
    }

    TestCase generate(int n) override {
        vector<Item> items(n);
        for (int i = 0; i < n; ++i) {
            items[i] = (n == 1) ? 0.5 : 0.1 + (0.8 * i) / (n - 1);
        }
        return TestCase(items, "increasing_n" + std::to_string(n),
                       "adversarial_increasing", 0);
    }
};
