#include <algorithm.h>

class BitmaskDP: BinPackingAlgorithm {
public:
    string name() const override {
        return "BitmaskDP";
    }

    Packing pack(const vector<Item> &items) override {
        int n = items.size();
        int N = 1 << n;  // Total number of subsets

        // dp[mask] = (bins used, filled capacity in the last bin) for subset represented by mask
        vector<pair<int, Item>> dp(N, {n + 1, MaxBinCapacity});
        
        vector<pair<int, Item>> parent(N, {-1, 0});  // To reconstruct the solution
        
        dp[0] = {0, MaxBinCapacity};
        parent[0] = {-1, 0};

        for(int mask = 0; mask < N; ++mask) {
            for(int i = 0; i < n; ++i) {
                if(mask & (1 << i)) continue;  // Item i is already in the subset

                int new_mask = mask | (1 << i);
                Item item = items[i];
                auto [bins_used, filled_capacity] = dp[mask];

                if(item + filled_capacity <= MaxBinCapacity) {
                    // Place item in the current bin
                    dp[new_mask] = min(dp[new_mask], {bins_used, filled_capacity + item});
                    parent[new_mask] = {mask, item};
                }
                else {
                    // Place item in a new bin
                    dp[new_mask] = min(dp[new_mask], {bins_used + 1, item});
                    parent[new_mask] = {mask, item};
                }
            }
        }

        Packing packing;
        int mask = N - 1;
        while(mask > 0) {
            auto [prev_mask, item] = parent[mask];
            if(prev_mask == -1) break;  // Reached the start

            if(dp[mask].first > dp[prev_mask].first) {
                // New bin was used
                Bin bin({item});
                packing.push_back(bin);
            }
            else {
                // Same bin, add to the last bin
                packing.back().push_back(item);
            }
            mask = prev_mask;
        }

        return packing;
    }
};