#ifndef QUERYLENS_EXECUTOR_HPP
#define QUERYLENS_EXECUTOR_HPP

#include "planner.hpp"
#include "../index/segment_tree.hpp"
#include "../index/lazy_segment_tree.hpp"
#include "../index/fenwick_tree.hpp"
#include "../index/bplus_tree.hpp"
#include "../storage/csv_loader.hpp"
#include <vector>
#include <string>
#include <memory>

namespace querylens {

// Result of executing a query
struct QueryResult {
    double result_value;          // Aggregate result
    double execution_time_ms;     // Query execution time
    ExecutionMethod method;       // Method used
    std::string method_name;      // Method name string
    bool success;
    std::string error_message;

    std::string toJSON() const;
};

// Query execution engine
// Wires together: parser → planner → index selection → aggregation
class QueryExecutor {
private:
    std::vector<int64_t> data_;
    int64_t data_size_;

    // Index structures (built on demand)
    std::unique_ptr<SegmentTree> segment_tree_;
    std::unique_ptr<LazySegmentTree> lazy_segment_tree_;
    std::unique_ptr<FenwickTree> fenwick_tree_;
    std::unique_ptr<BPlusTree> bplus_tree_;

    bool seg_built_, lazy_built_, fen_built_, bp_built_;

    // Internal execution methods
    double executeSequentialScan(const ParsedQuery& query) const;
    double executeSegmentTree(const ParsedQuery& query);
    double executeLazySegmentTree(const ParsedQuery& query);
    double executeFenwickTree(const ParsedQuery& query);
    double executeBPlusTree(const ParsedQuery& query);

public:
    QueryExecutor();
    
    // Load data from CSV
    void loadData(const std::string& filename);
    
    // Load data from vector
    void loadData(const std::vector<int64_t>& data);
    
    // Build specific indexes
    void buildSegmentTree();
    void buildLazySegmentTree();
    void buildFenwickTree();
    void buildBPlusTree();
    void buildAllIndexes();

    // Execute a query with automatic planning
    QueryResult execute(const std::string& sql);
    
    // Execute a query with a specific method
    QueryResult executeWith(const std::string& sql, ExecutionMethod method);

    // Get index build times
    double getSegmentTreeBuildTime();
    double getLazySegmentTreeBuildTime();
    double getFenwickTreeBuildTime();
    double getBPlusTreeBuildTime();

    // Get memory usage
    size_t getSegmentTreeMemory() const;
    size_t getLazySegmentTreeMemory() const;
    size_t getFenwickTreeMemory() const;
    size_t getBPlusTreeMemory() const;

    int64_t dataSize() const { return data_size_; }

    // For visualization: get tracked query execution
    std::string getTrackedExecution(const std::string& sql);
};

} // namespace querylens

#endif // QUERYLENS_EXECUTOR_HPP
