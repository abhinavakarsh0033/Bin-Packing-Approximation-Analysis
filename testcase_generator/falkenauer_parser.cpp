#include "generator.h"
#include <fstream>
#include <sstream>
#include <stdexcept>

// ============================================================================
// Falkenauer Benchmark Parser
// ============================================================================
// Parses benchmark files from Falkenauer's bin packing test suite.
// Two classes of instances:
//   - U class (Uniform): Items with integer sizes in [20, 100], bin capacity 150
//   - T class (Triplet): Items with float sizes, bin capacity 100.0
// ============================================================================

struct FalkenauerInstance {
    string name;
    double bin_capacity;
    int num_items;
    int optimal_bins;
    vector<Item> items;
    string instance_class;  // "U" or "T"
};

class FalkenauerParser {
public:
    // Parse a single benchmark file and return all instances
    static vector<FalkenauerInstance> parse_file(const string& filepath) {
        vector<FalkenauerInstance> instances;
        
        ifstream file(filepath);
        if (!file.is_open()) {
            throw runtime_error("Cannot open benchmark file: " + filepath);
        }
        
        // Read number of test cases
        int num_tests;
        file >> num_tests;
        
        for (int t = 0; t < num_tests; ++t) {
            FalkenauerInstance instance;
            
            // Read instance name
            file >> instance.name;
            
            // Determine class from name prefix
            instance.instance_class = (instance.name[0] == 'u' || instance.name[0] == 'U') ? "U" : "T";
            
            // Read parameters: bin_capacity num_items optimal_bins
            file >> instance.bin_capacity >> instance.num_items >> instance.optimal_bins;
            
            // Read items
            instance.items.resize(instance.num_items);
            for (int i = 0; i < instance.num_items; ++i) {
                double item_size;
                file >> item_size;
                // Normalize item size to [0, 1] by dividing by bin capacity
                instance.items[i] = item_size / instance.bin_capacity;
            }
            
            instances.push_back(instance);
        }
        
        file.close();
        return instances;
    }
    
    // Convert a Falkenauer instance to TestCase format
    static TestCase to_test_case(const FalkenauerInstance& instance) {
        string type = "Falkenauer_" + instance.instance_class;
        return TestCase(instance.items, instance.name, type, 0);
    }
    
    // Parse all benchmark files from a directory (expects binpack1.txt to binpack8.txt)
    static vector<FalkenauerInstance> parse_all(const string& benchmark_dir) {
        vector<FalkenauerInstance> all_instances;
        
        for (int i = 1; i <= 8; ++i) {
            string filepath = benchmark_dir + "/binpack" + to_string(i) + ".txt";
            try {
                vector<FalkenauerInstance> instances = parse_file(filepath);
                for (auto& inst : instances) {
                    all_instances.push_back(inst);
                }
            } catch (const exception& e) {
                // File may not exist, skip
                cerr << "Warning: " << e.what() << endl;
            }
        }
        
        return all_instances;
    }
};
