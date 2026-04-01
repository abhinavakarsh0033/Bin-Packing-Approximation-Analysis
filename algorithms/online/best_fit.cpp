#include "../../common/algorithm.h"

// ============================================================================
// Best Fit Online Algorithm
// ============================================================================
// Places each item in the bin with the smallest remaining space that still fits.
// Uses a multiset for O(log n) lookup of the best fitting bin.
// Time Complexity: O(n log n)
// Space Complexity: O(n)
// Approximation Ratio: 1.7 * OPT + 0.7
// ============================================================================

class BestFit : public BinPackingAlgorithm {
public:
    string name() const override {
        return "BestFit";
    }

    Packing pack(const vector<Item> &items) override {
        Packing packing;

        multiset<pair<Item, int>> bins;  // (remaining_capacity, bin_index)
        for(const Item &item: items) {
            // Find the best fit bin (remaining capacity after placing this item is minimum)
            auto it = bins.lower_bound({item, 0});

            // Found a bin that can accommodate the item
            if(it != bins.end()) {
                int bin_index = it->second;
                packing[bin_index].push_back(item);

                // Update the remaining capacity of the bin
                Item new_capacity = it->first - item;
                bins.erase(it);
                bins.insert({new_capacity, bin_index});
            }

            // No existing bin can accommodate this item
            else {
                // Open a new bin
                Bin new_bin({item});
                packing.push_back(new_bin);
                bins.insert({MaxBinCapacity - item, (int)packing.size() - 1});
            }
        }
        
        return packing;
    }
};