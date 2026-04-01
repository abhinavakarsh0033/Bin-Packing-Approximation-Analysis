#ifndef GENERATOR_H
#define GENERATOR_H

#include "../common/types.h"
#include <string>

// ============================================================================
// Test Case Generator Base Interface
// ============================================================================
// All generators must inherit from this class and implement generate().
// Similar pattern to BinPackingAlgorithm.
// ============================================================================

// Test case structure containing items and metadata
struct TestCase {
    vector<Item> items;
    string name;
    string type;
    int seed;

    TestCase() : seed(0) {}
    TestCase(const vector<Item>& its, const string& n, const string& t, int s = 0)
        : items(its), name(n), type(t), seed(s) {}
};

// Base class for all test case generators
class TestCaseGenerator {
public:
    // Returns the generator name
    virtual string name() const = 0;

    // Generate a test case with n items
    virtual TestCase generate(int n) = 0;

    // Virtual destructor
    virtual ~TestCaseGenerator() {}
};

#endif // GENERATOR_H
