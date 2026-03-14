#include "../../include/query/planner.hpp"
#include <sstream>

namespace querylens {

std::string QueryPlan::methodToString() const {
    switch (method) {
        case ExecutionMethod::SEQUENTIAL_SCAN:    return "Sequential Scan";
        case ExecutionMethod::SEGMENT_TREE:       return "Segment Tree";
        case ExecutionMethod::LAZY_SEGMENT_TREE:  return "Lazy Segment Tree";
        case ExecutionMethod::FENWICK_TREE:       return "Fenwick Tree";
        case ExecutionMethod::BPLUS_TREE:         return "B+ Tree";
        default:                                   return "Unknown";
    }
}

std::string QueryPlan::toJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"method\":\"" << methodToString() << "\",";
    oss << "\"explanation\":\"" << explanation << "\",";
    oss << "\"query\":" << QueryParser::toJSON(query);
    oss << "}";
    return oss.str();
}

QueryPlan QueryPlanner::plan(const ParsedQuery& query, bool has_segment_tree,
                              bool has_fenwick, bool has_bplus) {
    QueryPlan plan;
    plan.query = query;
    
    if (!query.valid) {
        plan.method = ExecutionMethod::SEQUENTIAL_SCAN;
        plan.explanation = "Invalid query; falling back to sequential scan";
        return plan;
    }
    
    // Decision logic:
    // 1. For SUM queries with range filter -> Segment Tree (best O(log N))
    // 2. For MIN/MAX with range filter -> Segment Tree (O(log N) vs O(N) for Fenwick)
    // 3. For COUNT -> Segment Tree (O(log N))
    // 4. For AVG -> Segment Tree (uses SUM + COUNT)
    // 5. If no range filter -> any method works, prefer Segment Tree
    
    if (query.has_filter) {
        switch (query.aggregate) {
            case AggFunc::SUM:
                if (has_segment_tree) {
                    plan.method = ExecutionMethod::SEGMENT_TREE;
                    plan.explanation = "Segment Tree selected for range SUM query (O(log N) complexity)";
                } else if (has_fenwick) {
                    plan.method = ExecutionMethod::FENWICK_TREE;
                    plan.explanation = "Fenwick Tree selected for range SUM query (O(log N) prefix sum)";
                } else {
                    plan.method = ExecutionMethod::SEQUENTIAL_SCAN;
                    plan.explanation = "No index available; using sequential scan O(N)";
                }
                break;
                
            case AggFunc::MIN:
            case AggFunc::MAX:
                if (has_segment_tree) {
                    plan.method = ExecutionMethod::SEGMENT_TREE;
                    plan.explanation = "Segment Tree selected for range MIN/MAX query (O(log N))";
                } else {
                    plan.method = ExecutionMethod::SEQUENTIAL_SCAN;
                    plan.explanation = "Sequential scan for MIN/MAX (Fenwick Tree does not support efficient range min/max)";
                }
                break;
                
            case AggFunc::COUNT:
            case AggFunc::AVG:
                if (has_segment_tree) {
                    plan.method = ExecutionMethod::SEGMENT_TREE;
                    plan.explanation = "Segment Tree selected for range aggregation (O(log N))";
                } else {
                    plan.method = ExecutionMethod::SEQUENTIAL_SCAN;
                    plan.explanation = "Sequential scan fallback";
                }
                break;
        }
    } else {
        // No range filter: full aggregation
        if (has_segment_tree) {
            plan.method = ExecutionMethod::SEGMENT_TREE;
            plan.explanation = "Segment Tree for full range aggregation";
        } else {
            plan.method = ExecutionMethod::SEQUENTIAL_SCAN;
            plan.explanation = "Sequential scan for full aggregation";
        }
    }
    
    return plan;
}

QueryPlan QueryPlanner::planWith(const ParsedQuery& query, ExecutionMethod method) {
    QueryPlan plan;
    plan.query = query;
    plan.method = method;
    
    switch (method) {
        case ExecutionMethod::SEQUENTIAL_SCAN:
            plan.explanation = "Forced: Sequential scan O(N)";
            break;
        case ExecutionMethod::SEGMENT_TREE:
            plan.explanation = "Forced: Segment Tree O(log N)";
            break;
        case ExecutionMethod::LAZY_SEGMENT_TREE:
            plan.explanation = "Forced: Lazy Segment Tree O(log N) with lazy propagation";
            break;
        case ExecutionMethod::FENWICK_TREE:
            plan.explanation = "Forced: Fenwick Tree O(log N) prefix sums";
            break;
        case ExecutionMethod::BPLUS_TREE:
            plan.explanation = "Forced: B+ Tree O(log N + K) range scan";
            break;
    }
    
    return plan;
}

} // namespace querylens
