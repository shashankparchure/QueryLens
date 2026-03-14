#include "../../include/index/fenwick_tree.hpp"
#include <algorithm>
#include <limits>

namespace querylens {

FenwickTree::FenwickTree() : n_(0) {}

FenwickTree::FenwickTree(const std::vector<int64_t>& data) {
    build(data);
}

void FenwickTree::build(const std::vector<int64_t>& data) {
    n_ = data.size();
    original_ = data;
    bit_.assign(n_ + 1, 0);
    
    // O(N) construction using the standard BIT build trick
    for (int i = 0; i < n_; i++) {
        int idx = i + 1; // 1-indexed
        bit_[idx] += data[i];
        int parent = idx + (idx & (-idx));
        if (parent <= n_) {
            bit_[parent] += bit_[idx];
        }
    }
}

void FenwickTree::update(int idx, int64_t delta) {
    if (idx < 0 || idx >= n_) return;
    original_[idx] += delta;
    idx++; // Convert to 1-indexed
    while (idx <= n_) {
        bit_[idx] += delta;
        idx += idx & (-idx);
    }
}

int64_t FenwickTree::prefixSum(int idx) const {
    if (idx < 0) return 0;
    if (idx >= n_) idx = n_ - 1;
    int64_t sum = 0;
    idx++; // Convert to 1-indexed
    while (idx > 0) {
        sum += bit_[idx];
        idx -= idx & (-idx);
    }
    return sum;
}

int64_t FenwickTree::rangeSum(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    if (l == 0) return prefixSum(r);
    return prefixSum(r) - prefixSum(l - 1);
}

int64_t FenwickTree::rangeMin(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    int64_t result = std::numeric_limits<int64_t>::max();
    for (int i = l; i <= r; i++) {
        result = std::min(result, original_[i]);
    }
    return result;
}

int64_t FenwickTree::rangeMax(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    int64_t result = std::numeric_limits<int64_t>::min();
    for (int i = l; i <= r; i++) {
        result = std::max(result, original_[i]);
    }
    return result;
}

double FenwickTree::rangeAvg(int l, int r) const {
    if (n_ == 0 || l > r) return 0.0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    int64_t cnt = r - l + 1;
    if (cnt == 0) return 0.0;
    return static_cast<double>(rangeSum(l, r)) / cnt;
}

size_t FenwickTree::memoryUsageBytes() const {
    return (bit_.capacity() + original_.capacity()) * sizeof(int64_t);
}

} // namespace querylens
