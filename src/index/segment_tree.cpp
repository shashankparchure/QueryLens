#include "../../include/index/segment_tree.hpp"
#include <sstream>
#include <algorithm>

namespace querylens {

SegmentTree::SegmentTree() : n_(0) {}

SegmentTree::SegmentTree(const std::vector<int64_t>& data) {
    build(data);
}

void SegmentTree::build(const std::vector<int64_t>& data) {
    n_ = data.size();
    if (n_ == 0) return;
    tree_sum_.assign(4 * n_, 0);
    tree_min_.assign(4 * n_, std::numeric_limits<int64_t>::max());
    tree_max_.assign(4 * n_, std::numeric_limits<int64_t>::min());
    buildTree(data, 1, 0, n_ - 1);
}

void SegmentTree::buildTree(const std::vector<int64_t>& data, int node, int start, int end) {
    if (start == end) {
        tree_sum_[node] = data[start];
        tree_min_[node] = data[start];
        tree_max_[node] = data[start];
        return;
    }
    int mid = (start + end) / 2;
    buildTree(data, 2 * node, start, mid);
    buildTree(data, 2 * node + 1, mid + 1, end);
    tree_sum_[node] = tree_sum_[2 * node] + tree_sum_[2 * node + 1];
    tree_min_[node] = std::min(tree_min_[2 * node], tree_min_[2 * node + 1]);
    tree_max_[node] = std::max(tree_max_[2 * node], tree_max_[2 * node + 1]);
}

int64_t SegmentTree::querySum(int node, int start, int end, int l, int r) const {
    if (r < start || end < l) return 0;
    if (l <= start && end <= r) return tree_sum_[node];
    int mid = (start + end) / 2;
    return querySum(2 * node, start, mid, l, r) +
           querySum(2 * node + 1, mid + 1, end, l, r);
}

int64_t SegmentTree::queryMin(int node, int start, int end, int l, int r) const {
    if (r < start || end < l) return std::numeric_limits<int64_t>::max();
    if (l <= start && end <= r) return tree_min_[node];
    int mid = (start + end) / 2;
    return std::min(queryMin(2 * node, start, mid, l, r),
                    queryMin(2 * node + 1, mid + 1, end, l, r));
}

int64_t SegmentTree::queryMax(int node, int start, int end, int l, int r) const {
    if (r < start || end < l) return std::numeric_limits<int64_t>::min();
    if (l <= start && end <= r) return tree_max_[node];
    int mid = (start + end) / 2;
    return std::max(queryMax(2 * node, start, mid, l, r),
                    queryMax(2 * node + 1, mid + 1, end, l, r));
}

void SegmentTree::updatePoint(int node, int start, int end, int idx, int64_t val) {
    if (start == end) {
        tree_sum_[node] = val;
        tree_min_[node] = val;
        tree_max_[node] = val;
        return;
    }
    int mid = (start + end) / 2;
    if (idx <= mid) updatePoint(2 * node, start, mid, idx, val);
    else updatePoint(2 * node + 1, mid + 1, end, idx, val);
    tree_sum_[node] = tree_sum_[2 * node] + tree_sum_[2 * node + 1];
    tree_min_[node] = std::min(tree_min_[2 * node], tree_min_[2 * node + 1]);
    tree_max_[node] = std::max(tree_max_[2 * node], tree_max_[2 * node + 1]);
}

int64_t SegmentTree::querySumTracked(int node, int start, int end, int l, int r,
                                      std::vector<VisitedNode>& visited) const {
    if (r < start || end < l) {
        visited.push_back({node, start, end, 0, "no_overlap"});
        return 0;
    }
    if (l <= start && end <= r) {
        visited.push_back({node, start, end, tree_sum_[node], "full_overlap"});
        return tree_sum_[node];
    }
    visited.push_back({node, start, end, tree_sum_[node], "partial"});
    int mid = (start + end) / 2;
    return querySumTracked(2 * node, start, mid, l, r, visited) +
           querySumTracked(2 * node + 1, mid + 1, end, l, r, visited);
}

// Public interface
int64_t SegmentTree::rangeSum(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return querySum(1, 0, n_ - 1, l, r);
}

int64_t SegmentTree::rangeMin(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return queryMin(1, 0, n_ - 1, l, r);
}

int64_t SegmentTree::rangeMax(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return queryMax(1, 0, n_ - 1, l, r);
}

int64_t SegmentTree::rangeCount(int l, int r) const {
    if (n_ == 0 || l > r) return 0;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    return r - l + 1;
}

double SegmentTree::rangeAvg(int l, int r) const {
    int64_t cnt = rangeCount(l, r);
    if (cnt == 0) return 0.0;
    return static_cast<double>(rangeSum(l, r)) / cnt;
}

SegmentTree::QueryResult SegmentTree::rangeQuery(int l, int r) const {
    return {rangeSum(l, r), rangeMin(l, r), rangeMax(l, r), rangeCount(l, r), rangeAvg(l, r)};
}

void SegmentTree::pointUpdate(int idx, int64_t val) {
    if (idx < 0 || idx >= n_) return;
    updatePoint(1, 0, n_ - 1, idx, val);
}

size_t SegmentTree::memoryUsageBytes() const {
    return (tree_sum_.capacity() + tree_min_.capacity() + tree_max_.capacity()) * sizeof(int64_t);
}

std::vector<SegmentTree::VisitedNode> SegmentTree::trackedRangeSum(int l, int r) const {
    std::vector<VisitedNode> visited;
    if (n_ == 0) return visited;
    l = std::max(l, 0);
    r = std::min(r, n_ - 1);
    querySumTracked(1, 0, n_ - 1, l, r, visited);
    return visited;
}

std::string SegmentTree::toJSON() const {
    if (n_ == 0) return "{}";
    std::ostringstream oss;
    oss << "{\"size\":" << n_ << ",\"nodes\":[";
    int total_nodes = std::min((int)tree_sum_.size(), 4 * n_);
    // Export node data for visualization (limit to reasonable size for frontend)
    int limit = std::min(total_nodes, 1024);
    for (int i = 1; i < limit; i++) {
        if (i > 1) oss << ",";
        oss << "{\"idx\":" << i
            << ",\"sum\":" << tree_sum_[i]
            << ",\"min\":" << tree_min_[i]
            << ",\"max\":" << tree_max_[i] << "}";
    }
    oss << "]}";
    return oss.str();
}

} // namespace querylens
