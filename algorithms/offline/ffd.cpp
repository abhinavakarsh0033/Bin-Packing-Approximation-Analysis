#include <algorithm.h>

class FFD: BinPackingAlgorithm {
public:
    string name() const override {
        return "FFD";
    }

    Packing pack(const vector<Item> &items) override {
        // Sort items in decreasing order
        vector<Item> sorted_items = items;
        sort(sorted_items.begin(), sorted_items.end(), greater<Item>());
        
        Packing packing;
        for(const Item &item: sorted_items) {
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