#include <algorithm.h>

class NextFit: BinPackingAlgorithm {
public:
    string name() const override {
        return "NextFit";
    }

    Packing pack(const vector<Item> &items) override {
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
                packing.push_back(current_bin);
                current_bin = Bin({item});
                curr_capacity = MaxBinCapacity - item;
            }
        }
        packing.push_back(current_bin);
        return packing;
    }
};