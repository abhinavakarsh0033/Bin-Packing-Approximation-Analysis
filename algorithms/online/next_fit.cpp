#include "../../common/algorithm.h"

// ============================================================================
// Next Fit Online Algorithm
// ============================================================================
// The simplest bin packing algorithm - only considers the current bin.
// If item doesn't fit, closes current bin and opens a new one.
// Time Complexity: O(n) - single pass
// Space Complexity: O(1) working memory
// Approximation Ratio: 2 * OPT (tight bound)
// ============================================================================

class NextFit : public BinPackingAlgorithm {
public:
    string name() const override {
        return "NextFit";
    }

    Packing pack(const vector<Item> &items) override {
        if (items.empty()) return {};

        Packing packing;
        Bin current_bin;
        Item curr_capacity = MaxBinCapacity;

        for(const Item &item: items) {
            // Try to fit the item in the current bin
            if(item <= curr_capacity) {
                current_bin.push_back(item);
                curr_capacity -= item;
            }
            else {  // Open a new bin
                if (!current_bin.empty()) {
                    packing.push_back(current_bin);
                }
                current_bin = Bin({item});
                curr_capacity = MaxBinCapacity - item;
            }
        }
        if (!current_bin.empty()) {
            packing.push_back(current_bin);
        }
        return packing;
    }
};