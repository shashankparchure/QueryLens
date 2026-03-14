#ifndef QUERYLENS_FENWICK_TREE_HPP
#define QUERYLENS_FENWICK_TREE_HPP

#include <vector>
#include <cstdint>

namespace querylens {

// Binary Indexed Tree (Fenwick Tree) for prefix sum queries
// Supports point update and prefix sum in O(log N)
// Range sum via prefixSum(r) - prefixSum(l-1)
class FenwickTree {
private:
    int n_;
    std::vector<int64_t> bit_;       // Binary indexed tree array
    std::vector<int64_t> original_;  // Original data for min/max fallback

public:
    FenwickTree();
    explicit FenwickTree(const std::vector<int64_t>& data);

    void build(const std::vector<int64_t>& data);
    
    // Point update: add delta to position idx (0-indexed)
    void update(int idx, int64_t delta);
    
    // Prefix sum: sum of elements [0, idx]
    int64_t prefixSum(int idx) const;
    
    // Range sum: sum of elements [l, r] (0-indexed)
    int64_t rangeSum(int l, int r) const;
    
    // Range min/max via linear scan of original data (O(N) for Fenwick)
    int64_t rangeMin(int l, int r) const;
    int64_t rangeMax(int l, int r) const;
    double rangeAvg(int l, int r) const;
    
    int size() const { return n_; }
    size_t memoryUsageBytes() const;
};

} // namespace querylens

#endif // QUERYLENS_FENWICK_TREE_HPP
