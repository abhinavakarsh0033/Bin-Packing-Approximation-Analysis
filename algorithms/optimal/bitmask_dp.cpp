#include "../../common/algorithm.h"

// ============================================================================
// Bitmask DP Optimal Bin Packing
// ============================================================================
// Finds the optimal solution using dynamic programming over subsets.
// State: dp[mask] = (min_bins, remaining_capacity_in_last_bin) for items in mask
// Transition: For each item not in mask, try adding it to current or new bin
// Time Complexity: O(n * 2^n)
// Space Complexity: O(2^n)
// Only suitable for small inputs (N <= 20)
// ============================================================================

class BitmaskDP : public BinPackingAlgorithm {
public:
    string name() const override {
        return "BitmaskDP";
    }

    Packing pack(const vector<Item>& items) override {
        int n = items.size();
        if (n == 0) return {};
        if (n > 20) {
            // Too many items for bitmask DP - fallback or error
            throw std::runtime_error("BitmaskDP: Too many items (max 20)");
        }

        int N = 1 << n;  // Total number of subsets

        // dp[mask] = (bins_used, remaining_capacity in current bin)
        // We want to minimize bins_used, and maximize remaining_capacity as tiebreaker
        // Using negative remaining so min works correctly
        vector<pair<int, Item>> dp(N, {n + 1, 0.0});

        // parent[mask] = (prev_mask, item_index) for reconstruction
        vector<pair<int, int>> parent(N, {-1, -1});

        // Base case: empty set needs 0 bins
        dp[0] = {0, MaxBinCapacity};  // 0 bins, full capacity available

        for (int mask = 0; mask < N; ++mask) {
            if (dp[mask].first > n) continue;  // Invalid state

            for (int i = 0; i < n; ++i) {
                if (mask & (1 << i)) continue;  // Item i already in subset

                int new_mask = mask | (1 << i);
                Item item = items[i];
                auto [bins_used, remaining] = dp[mask];

                pair<int, Item> new_state;
                if (item <= remaining + 1e-9) {
                    // Place item in current bin
                    new_state = {bins_used, remaining - item};
                } else {
                    // Need a new bin
                    new_state = {bins_used + 1, MaxBinCapacity - item};
                }

                // Update if better: fewer bins, or same bins but more remaining space
                if (new_state.first < dp[new_mask].first ||
                    (new_state.first == dp[new_mask].first &&
                     new_state.second > dp[new_mask].second)) {
                    dp[new_mask] = new_state;
                    parent[new_mask] = {mask, i};
                }
            }
        }

        // Reconstruct the solution
        // We need to track which bin each item goes into
        Packing packing;
        int mask = N - 1;

        // Trace back through parent pointers
        vector<pair<int, int>> trace;  // (mask, item_index) in reverse order
        while (mask > 0) {
            auto [prev_mask, item_idx] = parent[mask];
            if (item_idx == -1) break;
            trace.push_back({mask, item_idx});
            mask = prev_mask;
        }

        // Process items in forward order (reverse of trace)
        for (int t = trace.size() - 1; t >= 0; --t) {
            int curr_mask = trace[t].first;
            int prev_mask = (t + 1 < (int)trace.size()) ? trace[t + 1].first : 0;
            if (t == (int)trace.size() - 1) {
                prev_mask = parent[curr_mask].first;
            }
            int item_idx = trace[t].second;
            Item item = items[item_idx];

            // Check if this item started a new bin
            if (dp[curr_mask].first > dp[prev_mask].first) {
                // New bin was opened
                packing.push_back({item});
            } else {
                // Item added to existing bin
                if (packing.empty()) {
                    packing.push_back({item});
                } else {
                    packing.back().push_back(item);
                }
            }
        }

        return packing;
    }
};
