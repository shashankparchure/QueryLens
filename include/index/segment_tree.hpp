#ifndef QUERYLENS_SEGMENT_TREE_HPP
#define QUERYLENS_SEGMENT_TREE_HPP

#include <vector>
#include <limits>
#include <cstdint>
#include <string>

namespace querylens {

// Array-based segment tree for efficient range aggregate queries
// Supports SUM, MIN, MAX, COUNT operations in O(log N) time
class SegmentTree {
public:
    enum class AggregateType { SUM, MIN, MAX, COUNT, AVG };

    struct QueryResult {
        int64_t sum;
        int64_t min_val;
        int64_t max_val;
        int64_t count;
        double avg;
    };

    // Node visited during a query (for visualization)
    struct VisitedNode {
        int node_idx;
        int seg_left;
        int seg_right;
        int64_t value;
        std::string action; // "full_overlap", "partial", "no_overlap"
    };

private:
    int n_;                          // Number of elements
    std::vector<int64_t> tree_sum_;  // Segment tree for sum
    std::vector<int64_t> tree_min_;  // Segment tree for min
    std::vector<int64_t> tree_max_;  // Segment tree for max

    void buildTree(const std::vector<int64_t>& data, int node, int start, int end);
    int64_t querySum(int node, int start, int end, int l, int r) const;
    int64_t queryMin(int node, int start, int end, int l, int r) const;
    int64_t queryMax(int node, int start, int end, int l, int r) const;
    void updatePoint(int node, int start, int end, int idx, int64_t val);

    // Query with path tracking for visualization
    int64_t querySumTracked(int node, int start, int end, int l, int r,
                            std::vector<VisitedNode>& visited) const;

public:
    SegmentTree();
    explicit SegmentTree(const std::vector<int64_t>& data);

    void build(const std::vector<int64_t>& data);
    
    int64_t rangeSum(int l, int r) const;
    int64_t rangeMin(int l, int r) const;
    int64_t rangeMax(int l, int r) const;
    int64_t rangeCount(int l, int r) const;
    double rangeAvg(int l, int r) const;
    
    QueryResult rangeQuery(int l, int r) const;
    
    void pointUpdate(int idx, int64_t val);
    
    int size() const { return n_; }
    size_t memoryUsageBytes() const;

    // For visualization: returns list of nodes visited during a sum query
    std::vector<VisitedNode> trackedRangeSum(int l, int r) const;

    // Export tree structure as JSON for frontend
    std::string toJSON() const;
};

} // namespace querylens

#endif // QUERYLENS_SEGMENT_TREE_HPP
