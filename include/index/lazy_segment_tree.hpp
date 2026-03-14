#ifndef QUERYLENS_LAZY_SEGMENT_TREE_HPP
#define QUERYLENS_LAZY_SEGMENT_TREE_HPP

#include <vector>
#include <limits>
#include <cstdint>
#include <string>

namespace querylens {

// Segment Tree with Lazy Propagation for efficient range updates
// Range update + Range query both in O(log N)
class LazySegmentTree {
public:
    // Animation step for lazy propagation visualization
    struct PropagationStep {
        int node_idx;
        int seg_left;
        int seg_right;
        int64_t lazy_value;
        std::string action; // "push_down", "update_node", "query_node"
    };

private:
    int n_;
    std::vector<int64_t> tree_;    // Sum tree
    std::vector<int64_t> lazy_;    // Lazy propagation buffer
    std::vector<int64_t> tree_min_;
    std::vector<int64_t> tree_max_;
    std::vector<int64_t> lazy_min_;
    std::vector<int64_t> lazy_max_;

    void buildTree(const std::vector<int64_t>& data, int node, int start, int end);
    void pushDown(int node, int start, int end);
    void rangeUpdate(int node, int start, int end, int l, int r, int64_t val);
    int64_t querySum(int node, int start, int end, int l, int r);
    int64_t queryMin(int node, int start, int end, int l, int r);
    int64_t queryMax(int node, int start, int end, int l, int r);

    // Tracked versions for visualization
    void rangeUpdateTracked(int node, int start, int end, int l, int r, int64_t val,
                            std::vector<PropagationStep>& steps);
    int64_t querySumTracked(int node, int start, int end, int l, int r,
                            std::vector<PropagationStep>& steps);

public:
    LazySegmentTree();
    explicit LazySegmentTree(const std::vector<int64_t>& data);

    void build(const std::vector<int64_t>& data);
    
    // Range update: add val to all elements in [l, r]
    void rangeUpdate(int l, int r, int64_t val);
    
    int64_t rangeSum(int l, int r);
    int64_t rangeMin(int l, int r);
    int64_t rangeMax(int l, int r);
    double rangeAvg(int l, int r);

    int size() const { return n_; }
    size_t memoryUsageBytes() const;

    // Visualization support
    std::vector<PropagationStep> trackedRangeUpdate(int l, int r, int64_t val);
    std::vector<PropagationStep> trackedRangeSum(int l, int r);
    std::string toJSON() const;
};

} // namespace querylens

#endif // QUERYLENS_LAZY_SEGMENT_TREE_HPP
