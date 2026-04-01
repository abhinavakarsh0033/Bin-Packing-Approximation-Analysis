#include "../../common/algorithm.h"

// ============================================================================
// Best Fit Decreasing (BFD) Algorithm
// ============================================================================
// Offline algorithm that sorts items in decreasing order, then applies Best Fit.
// Uses a multiset for O(log n) bin lookup.
// Time Complexity: O(n log n)
// Space Complexity: O(n)
// Approximation Ratio: 11/9 * OPT + 6/9 (approximately 1.222 * OPT)
// ============================================================================

class BFD : public BinPackingAlgorithm {
public:
    string name() const override {
        return "BFD";
    }

    Packing pack(const vector<Item> &items) override {
        // Sort items in decreasing order
        vector<Item> sorted_items = items;
        sort(sorted_items.begin(), sorted_items.end(), greater<Item>());
        
        Packing packing;
        multiset<pair<Item, int>> bins;  // (remaining_capacity, bin_index)
        for(const Item &item: sorted_items) {
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