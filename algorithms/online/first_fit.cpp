#include "../../common/algorithm.h"

// ============================================================================
// First Fit Online Algorithm
// ============================================================================
// Places each item in the first bin that has enough room.
// Time Complexity: O(n^2) - linear scan for each item
// Space Complexity: O(n)
// Approximation Ratio: 1.7 * OPT + 0.7
// ============================================================================

class FirstFit : public BinPackingAlgorithm {
public:
    string name() const override {
        return "FirstFit";
    }

    Packing pack(const vector<Item> &items) override {
        Packing packing;

        for(const Item &item: items) {
            bool placed = false;

            // Check if the item can fit in any of the existing bins
            for(Bin &bin: packing) {
                Item remaining_capacity = MaxBinCapacity - accumulate(bin.begin(), bin.end(), 0.0);
                if(item <= remaining_capacity) {
                    bin.push_back(item);
                    placed = true;
                    break;
                }
            }

            // No existing bin can accommodate this item
            if(!placed) {
                // Open a new bin for this item
                Bin bin({item});
                packing.push_back(bin);
            }
        }

        return packing;
    }
};