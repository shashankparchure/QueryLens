#include "../../include/index/lazy_segment_tree.hpp"
#include <sstream>
#include <algorithm>

namespace querylens {

LazySegmentTree::LazySegmentTree() : n_(0) {}

LazySegmentTree::LazySegmentTree(const std::vector<int64_t>& data) {
    build(data);
}

void LazySegmentTree::build(const std::vector<int64_t>& data) {
    n_ = data.size();
    if (n_ == 0) return;
    tree_.assign(4 * n_, 0);
    lazy_.assign(4 * n_, 0);
    tree_min_.assign(4 * n_, std::numeric_limits<int64_t>::max());
    tree_max_.assign(4 * n_, std::numeric_limits<int64_t>::min());
    lazy_min_.assign(4 * n_, 0);
    lazy_max_.assign(4 * n_, 0);
    buildTree(data, 1, 0, n_ - 1);
}

void LazySegmentTree::buildTree(const std::vector<int64_t>& data, int node, int start, int end) {
    if (start == end) {
        tree_[node] = data[start];
        tree_min_[node] = data[start];
        tree_max_[node] = data[start];
        return;
    }
    int mid = (start + end) / 2;
    buildTree(data, 2 * node, start, mid);
    buildTree(data, 2 * node + 1, mid + 1, end);
    tree_[node] = tree_[2 * node] + tree_[2 * node + 1];
    tree_min_[node] = std::min(tree_min_[2 * node], tree_min_[2 * node + 1]);
    tree_max_[node] = std::max(tree_max_[2 * node], tree_max_[2 * node + 1]);
}

void LazySegmentTree::pushDown(int node, int start, int end) {
    if (lazy_[node] != 0) {
        int mid = (start + end) / 2;
        int left = 2 * node, right = 2 * node + 1;
        
        // Push to left child
        tree_[left] += lazy_[node] * (mid - start + 1);
        tree_min_[left] += lazy_[node];
        tree_max_[left] += lazy_[node];
        lazy_[left] += lazy_[node];
        lazy_min_[left] += lazy_[node];
        lazy_max_[left] += lazy_[node];
        
        // Push to right child
        tree_[right] += lazy_[node] * (end - mid);
        tree_min_[right] += lazy_[node];
        tree_max_[right] += lazy_[node];
        lazy_[right] += lazy_[node];
        lazy_min_[right] += lazy_[node];
        lazy_max_[right] += lazy_[node];
        
        lazy_[node] = 0;
    }
}

void LazySegmentTree::rangeUpdate(int node, int start, int end, int l, int r, int64_t val) {
    if (r < start || end < l) return;
    if (l <= start && end <= r) {
        tree_[node] += val * (end - start + 1);
        tree_min_[node] += val;
        tree_max_[node] += val;
        lazy_[node] += val;
        lazy_min_[node] += val;
        lazy_max_[node] += val;
        return;
    }
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    rangeUpdate(2 * node, start, mid, l, r, val);
    rangeUpdate(2 * node + 1, mid + 1, end, l, r, val);
    tree_[node] = tree_[2 * node] + tree_[2 * node + 1];
    tree_min_[node] = std::min(tree_min_[2 * node], tree_min_[2 * node + 1]);
    tree_max_[node] = std::max(tree_max_[2 * node], tree_max_[2 * node + 1]);
}

int64_t LazySegmentTree::querySum(int node, int start, int end, int l, int r) {
    if (r < start || end < l) return 0;
    if (l <= start && end <= r) return tree_[node];
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    return querySum(2 * node, start, mid, l, r) +
           querySum(2 * node + 1, mid + 1, end, l, r);
}

int64_t LazySegmentTree::queryMin(int node, int start, int end, int l, int r) {
    if (r < start || end < l) return std::numeric_limits<int64_t>::max();
    if (l <= start && end <= r) return tree_min_[node];
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    return std::min(queryMin(2 * node, start, mid, l, r),
                    queryMin(2 * node + 1, mid + 1, end, l, r));
}

int64_t LazySegmentTree::queryMax(int node, int start, int end, int l, int r) {
    if (r < start || end < l) return std::numeric_limits<int64_t>::min();
    if (l <= start && end <= r) return tree_max_[node];
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    return std::max(queryMax(2 * node, start, mid, l, r),
                    queryMax(2 * node + 1, mid + 1, end, l, r));
}

// Tracked versions for visualization
void LazySegmentTree::rangeUpdateTracked(int node, int start, int end, int l, int r, 
                                          int64_t val, std::vector<PropagationStep>& steps) {
    if (r < start || end < l) return;
    if (l <= start && end <= r) {
        tree_[node] += val * (end - start + 1);
        tree_min_[node] += val;
        tree_max_[node] += val;
        lazy_[node] += val;
        steps.push_back({node, start, end, lazy_[node], "update_node"});
        return;
    }
    if (lazy_[node] != 0) {
        steps.push_back({node, start, end, lazy_[node], "push_down"});
    }
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    rangeUpdateTracked(2 * node, start, mid, l, r, val, steps);
    rangeUpdateTracked(2 * node + 1, mid + 1, end, l, r, val, steps);
    tree_[node] = tree_[2 * node] + tree_[2 * node + 1];
    tree_min_[node] = std::min(tree_min_[2 * node], tree_min_[2 * node + 1]);
    tree_max_[node] = std::max(tree_max_[2 * node], tree_max_[2 * node + 1]);
}

int64_t LazySegmentTree::querySumTracked(int node, int start, int end, int l, int r,
                                          std::vector<PropagationStep>& steps) {
    if (r < start || end < l) return 0;
    if (l <= start && end <= r) {
        steps.push_back({node, start, end, tree_[node], "query_node"});
        return tree_[node];
    }
    if (lazy_[node] != 0) {
        steps.push_back({node, start, end, lazy_[node], "push_down"});
    }
    pushDown(node, start, end);
    int mid = (start + end) / 2;
    return querySumTracked(2 * node, start, mid, l, r, steps) +
           querySumTracked(2 * node + 1, mid + 1, end, l, r, steps);
}

// Public interface
void LazySegmentTree::rangeUpdate(int l, int r, int64_t val) {
    if (n_ == 0 || l > r) return;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    rangeUpdate(1, 0, n_ - 1, l, r, val);
}

int64_t LazySegmentTree::rangeSum(int l, int r) {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return querySum(1, 0, n_ - 1, l, r);
}

int64_t LazySegmentTree::rangeMin(int l, int r) {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return queryMin(1, 0, n_ - 1, l, r);
}

int64_t LazySegmentTree::rangeMax(int l, int r) {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return queryMax(1, 0, n_ - 1, l, r);
}

double LazySegmentTree::rangeAvg(int l, int r) {
    if (n_ == 0 || l > r) return 0.0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    int64_t cnt = r - l + 1;
    if (cnt == 0) return 0.0;
    return static_cast<double>(rangeSum(l, r)) / cnt;
}

size_t LazySegmentTree::memoryUsageBytes() const {
    return (tree_.capacity() + lazy_.capacity() + 
            tree_min_.capacity() + tree_max_.capacity() +
            lazy_min_.capacity() + lazy_max_.capacity()) * sizeof(int64_t);
}

std::vector<LazySegmentTree::PropagationStep> LazySegmentTree::trackedRangeUpdate(int l, int r, int64_t val) {
    std::vector<PropagationStep> steps;
    if (n_ == 0) return steps;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    rangeUpdateTracked(1, 0, n_ - 1, l, r, val, steps);
    return steps;
}

std::vector<LazySegmentTree::PropagationStep> LazySegmentTree::trackedRangeSum(int l, int r) {
    std::vector<PropagationStep> steps;
    if (n_ == 0) return steps;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    querySumTracked(1, 0, n_ - 1, l, r, steps);
    return steps;
}

std::string LazySegmentTree::toJSON() const {
    if (n_ == 0) return "{}";
    std::ostringstream oss;
    oss << "{\"size\":" << n_ << ",\"nodes\":[";
    int limit = std::min((int)tree_.size(), std::min(4 * n_, 1024));
    for (int i = 1; i < limit; i++) {
        if (i > 1) oss << ",";
        oss << "{\"idx\":" << i
            << ",\"sum\":" << tree_[i]
            << ",\"lazy\":" << lazy_[i]
            << ",\"min\":" << tree_min_[i]
            << ",\"max\":" << tree_max_[i] << "}";
    }
    oss << "]}";
    return oss.str();
}

} // namespace querylens
