#include "../../common/algorithm.h"

// ============================================================================
// Brute Force Optimal Bin Packing
// ============================================================================
// Finds the optimal solution by trying all possible partitions of items into bins.
// Uses backtracking with pruning to reduce the search space.
// Time Complexity: O(n! * 2^n) worst case, but pruning helps significantly
// Space Complexity: O(n) for recursion stack
// Only suitable for small inputs (N <= 15)
// ============================================================================

class BruteForce : public BinPackingAlgorithm {
private:
    int best_bins;
    Packing best_packing;

    // Backtracking function to try all possible placements
    // current_packing: bins used so far with their remaining capacities
    // items: all items to pack
    // index: current item index being placed
    void backtrack(vector<pair<Bin, Item>>& current_packing,
                   const vector<Item>& items,
                   int index) {
        // Pruning: if we already have more bins than best, stop
        if ((int)current_packing.size() >= best_bins) {
            return;
        }

        // Base case: all items placed
        if (index == (int)items.size()) {
            if ((int)current_packing.size() < best_bins) {
                best_bins = current_packing.size();
                best_packing.clear();
                for (const auto& [bin, remaining] : current_packing) {
                    best_packing.push_back(bin);
                }
            }
            return;
        }

        Item item = items[index];

        // Try to place item in each existing bin
        for (size_t i = 0; i < current_packing.size(); ++i) {
            if (current_packing[i].second >= item) {
                // Place item in bin i
                current_packing[i].first.push_back(item);
                current_packing[i].second -= item;

                backtrack(current_packing, items, index + 1);

                // Remove item from bin i (backtrack)
                current_packing[i].first.pop_back();
                current_packing[i].second += item;
            }
        }

        // Try placing item in a new bin (only if it helps)
        if ((int)current_packing.size() + 1 < best_bins) {
            current_packing.push_back({{item}, MaxBinCapacity - item});
            backtrack(current_packing, items, index + 1);
            current_packing.pop_back();
        }
    }

public:
    string name() const override {
        return "BruteForce";
    }

    Packing pack(const vector<Item>& items) override {
        if (items.empty()) {
            return {};
        }

        // Initialize best solution to worst case (each item in its own bin)
        best_bins = items.size() + 1;
        best_packing.clear();

        // Sort items in decreasing order for better pruning
        // Larger items first constrains the search space more quickly
        vector<Item> sorted_items = items;
        sort(sorted_items.begin(), sorted_items.end(), greater<Item>());

        // Start backtracking
        vector<pair<Bin, Item>> current_packing;
        backtrack(current_packing, sorted_items, 0);

        return best_packing;
    }
};
