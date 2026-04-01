#include "generator.h"
#include <random>

// ============================================================================
// Random Test Case Generators
// ============================================================================
// Concrete implementations of TestCaseGenerator for random distributions.
// ============================================================================

// Uniform distribution generator
class UniformGenerator : public TestCaseGenerator {
private:
    std::mt19937 rng;
    double min_size, max_size;
    int seed;

public:
    UniformGenerator(double min_s = 0.1, double max_s = 0.9, int s = 42)
        : rng(s), min_size(min_s), max_size(max_s), seed(s) {}

    string name() const override {
        return "Uniform";
    }

    TestCase generate(int n) override {
        std::uniform_real_distribution<double> dist(min_size, max_size);
        vector<Item> items(n);

        for (int i = 0; i < n; ++i) {
            items[i] = dist(rng);
        }

        return TestCase(items, "uniform_n" + std::to_string(n),
                       "random_uniform", seed);
    }

    void set_seed(int s) {
        seed = s;
        rng.seed(s);
    }
};

// Normal distribution generator
class NormalGenerator : public TestCaseGenerator {
private:
    std::mt19937 rng;
    double mean, stddev, min_val, max_val;
    int seed;

public:
    NormalGenerator(double m = 0.5, double s = 0.2,
                    double min_v = 0.05, double max_v = 0.95, int sd = 42)
        : rng(sd), mean(m), stddev(s), min_val(min_v), max_val(max_v), seed(sd) {}

    string name() const override {
        return "Normal";
    }

    TestCase generate(int n) override {
        std::normal_distribution<double> dist(mean, stddev);
        vector<Item> items(n);

        for (int i = 0; i < n; ++i) {
            double val = dist(rng);
            val = std::max(min_val, std::min(max_val, val));
            items[i] = val;
        }

        return TestCase(items, "normal_n" + std::to_string(n),
                       "random_normal", seed);
    }

    void set_seed(int s) {
        seed = s;
        rng.seed(s);
    }
};

// Exponential distribution generator
class ExponentialGenerator : public TestCaseGenerator {
private:
    std::mt19937 rng;
    double lambda, min_val, max_val;
    int seed;

public:
    ExponentialGenerator(double l = 2.0, double min_v = 0.05, double max_v = 0.95, int s = 42)
        : rng(s), lambda(l), min_val(min_v), max_val(max_v), seed(s) {}

    string name() const override {
        return "Exponential";
    }

    TestCase generate(int n) override {
        std::exponential_distribution<double> dist(lambda);
        vector<Item> items(n);

        for (int i = 0; i < n; ++i) {
            double val = dist(rng);
            val = min_val + (max_val - min_val) * (1.0 - std::exp(-val));
            val = std::max(min_val, std::min(max_val, val));
            items[i] = val;
        }

        return TestCase(items, "exponential_n" + std::to_string(n),
                       "random_exponential", seed);
    }

    void set_seed(int s) {
        seed = s;
        rng.seed(s);
    }
};
