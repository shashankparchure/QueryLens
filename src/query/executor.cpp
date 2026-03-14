#include "../../include/query/executor.hpp"
#include "../../include/utils/timer.hpp"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <numeric>

namespace querylens {

std::string QueryResult::toJSON() const {
    std::ostringstream oss;
    oss << "{";
    oss << "\"result\":" << result_value << ",";
    oss << "\"execution_time_ms\":" << execution_time_ms << ",";
    oss << "\"method\":\"" << method_name << "\",";
    oss << "\"success\":" << (success ? "true" : "false");
    if (!error_message.empty()) {
        oss << ",\"error\":\"" << error_message << "\"";
    }
    oss << "}";
    return oss.str();
}

QueryExecutor::QueryExecutor()
    : data_size_(0), seg_built_(false), lazy_built_(false),
      fen_built_(false), bp_built_(false) {}

void QueryExecutor::loadData(const std::string& filename) {
    data_ = CSVLoader::loadValues(filename);
    data_size_ = data_.size();
    seg_built_ = lazy_built_ = fen_built_ = bp_built_ = false;
}

void QueryExecutor::loadData(const std::vector<int64_t>& data) {
    data_ = data;
    data_size_ = data.size();
    seg_built_ = lazy_built_ = fen_built_ = bp_built_ = false;
}

void QueryExecutor::buildSegmentTree() {
    segment_tree_ = std::make_unique<SegmentTree>(data_);
    seg_built_ = true;
}

void QueryExecutor::buildLazySegmentTree() {
    lazy_segment_tree_ = std::make_unique<LazySegmentTree>(data_);
    lazy_built_ = true;
}

void QueryExecutor::buildFenwickTree() {
    fenwick_tree_ = std::make_unique<FenwickTree>(data_);
    fen_built_ = true;
}

void QueryExecutor::buildBPlusTree() {
    bplus_tree_ = std::make_unique<BPlusTree>();
    std::vector<BPlusTree::Record> records;
    records.reserve(data_.size());
    for (int64_t i = 0; i < (int64_t)data_.size(); i++) {
        records.push_back({i, data_[i]});
    }
    bplus_tree_->build(records);
    bp_built_ = true;
}

void QueryExecutor::buildAllIndexes() {
    buildSegmentTree();
    buildLazySegmentTree();
    buildFenwickTree();
    buildBPlusTree();
}

double QueryExecutor::executeSequentialScan(const ParsedQuery& query) const {
    int64_t low = query.has_filter ? query.range_low : 0;
    int64_t high = query.has_filter ? query.range_high : data_size_ - 1;
    
    low = std::max(low, (int64_t)0);
    high = std::min(high, data_size_ - 1);
    
    switch (query.aggregate) {
        case AggFunc::SUM: {
            int64_t sum = 0;
            for (int64_t i = low; i <= high; i++) sum += data_[i];
            return sum;
        }
        case AggFunc::MIN: {
            int64_t min_val = std::numeric_limits<int64_t>::max();
            for (int64_t i = low; i <= high; i++) min_val = std::min(min_val, data_[i]);
            return min_val;
        }
        case AggFunc::MAX: {
            int64_t max_val = std::numeric_limits<int64_t>::min();
            for (int64_t i = low; i <= high; i++) max_val = std::max(max_val, data_[i]);
            return max_val;
        }
        case AggFunc::COUNT: {
            return high - low + 1;
        }
        case AggFunc::AVG: {
            int64_t sum = 0;
            int64_t count = high - low + 1;
            for (int64_t i = low; i <= high; i++) sum += data_[i];
            return count > 0 ? (double)sum / count : 0.0;
        }
    }
    return 0;
}

double QueryExecutor::executeSegmentTree(const ParsedQuery& query) {
    if (!seg_built_) buildSegmentTree();
    
    int l = query.has_filter ? query.range_low : 0;
    int r = query.has_filter ? query.range_high : data_size_ - 1;
    
    switch (query.aggregate) {
        case AggFunc::SUM:   return segment_tree_->rangeSum(l, r);
        case AggFunc::MIN:   return segment_tree_->rangeMin(l, r);
        case AggFunc::MAX:   return segment_tree_->rangeMax(l, r);
        case AggFunc::COUNT: return segment_tree_->rangeCount(l, r);
        case AggFunc::AVG:   return segment_tree_->rangeAvg(l, r);
    }
    return 0;
}

double QueryExecutor::executeLazySegmentTree(const ParsedQuery& query) {
    if (!lazy_built_) buildLazySegmentTree();
    
    int l = query.has_filter ? query.range_low : 0;
    int r = query.has_filter ? query.range_high : data_size_ - 1;
    
    switch (query.aggregate) {
        case AggFunc::SUM:   return lazy_segment_tree_->rangeSum(l, r);
        case AggFunc::MIN:   return lazy_segment_tree_->rangeMin(l, r);
        case AggFunc::MAX:   return lazy_segment_tree_->rangeMax(l, r);
        case AggFunc::COUNT: return r - l + 1;
        case AggFunc::AVG:   return lazy_segment_tree_->rangeAvg(l, r);
    }
    return 0;
}

double QueryExecutor::executeFenwickTree(const ParsedQuery& query) {
    if (!fen_built_) buildFenwickTree();
    
    int l = query.has_filter ? query.range_low : 0;
    int r = query.has_filter ? query.range_high : data_size_ - 1;
    
    switch (query.aggregate) {
        case AggFunc::SUM:   return fenwick_tree_->rangeSum(l, r);
        case AggFunc::MIN:   return fenwick_tree_->rangeMin(l, r);
        case AggFunc::MAX:   return fenwick_tree_->rangeMax(l, r);
        case AggFunc::COUNT: return r - l + 1;
        case AggFunc::AVG:   return fenwick_tree_->rangeAvg(l, r);
    }
    return 0;
}

double QueryExecutor::executeBPlusTree(const ParsedQuery& query) {
    if (!bp_built_) buildBPlusTree();
    
    int64_t low = query.has_filter ? query.range_low : 0;
    int64_t high = query.has_filter ? query.range_high : data_size_ - 1;
    
    switch (query.aggregate) {
        case AggFunc::SUM:   return bplus_tree_->rangeSum(low, high);
        case AggFunc::MIN:   return bplus_tree_->rangeMin(low, high);
        case AggFunc::MAX:   return bplus_tree_->rangeMax(low, high);
        case AggFunc::COUNT: return bplus_tree_->rangeCount(low, high);
        case AggFunc::AVG:   return bplus_tree_->rangeAvg(low, high);
    }
    return 0;
}

QueryResult QueryExecutor::execute(const std::string& sql) {
    ParsedQuery parsed = QueryParser::parse(sql);
    if (!parsed.valid) {
        return {0, 0, ExecutionMethod::SEQUENTIAL_SCAN, "Error", false, parsed.error_message};
    }
    
    QueryPlan plan = QueryPlanner::plan(parsed, seg_built_ || true, fen_built_ || true, bp_built_ || true);
    return executeWith(sql, plan.method);
}

QueryResult QueryExecutor::executeWith(const std::string& sql, ExecutionMethod method) {
    QueryResult result;
    
    ParsedQuery parsed = QueryParser::parse(sql);
    if (!parsed.valid) {
        result.success = false;
        result.error_message = parsed.error_message;
        return result;
    }
    
    Timer timer;
    timer.start();
    
    switch (method) {
        case ExecutionMethod::SEQUENTIAL_SCAN:
            result.result_value = executeSequentialScan(parsed);
            result.method_name = "Sequential Scan";
            break;
        case ExecutionMethod::SEGMENT_TREE:
            result.result_value = executeSegmentTree(parsed);
            result.method_name = "Segment Tree";
            break;
        case ExecutionMethod::LAZY_SEGMENT_TREE:
            result.result_value = executeLazySegmentTree(parsed);
            result.method_name = "Lazy Segment Tree";
            break;
        case ExecutionMethod::FENWICK_TREE:
            result.result_value = executeFenwickTree(parsed);
            result.method_name = "Fenwick Tree";
            break;
        case ExecutionMethod::BPLUS_TREE:
            result.result_value = executeBPlusTree(parsed);
            result.method_name = "B+ Tree";
            break;
    }
    
    timer.stop();
    result.execution_time_ms = timer.elapsed_ms();
    result.method = method;
    result.success = true;
    
    return result;
}

double QueryExecutor::getSegmentTreeBuildTime() {
    Timer t;
    t.start();
    segment_tree_ = std::make_unique<SegmentTree>(data_);
    t.stop();
    seg_built_ = true;
    return t.elapsed_ms();
}

double QueryExecutor::getLazySegmentTreeBuildTime() {
    Timer t;
    t.start();
    lazy_segment_tree_ = std::make_unique<LazySegmentTree>(data_);
    t.stop();
    lazy_built_ = true;
    return t.elapsed_ms();
}

double QueryExecutor::getFenwickTreeBuildTime() {
    Timer t;
    t.start();
    fenwick_tree_ = std::make_unique<FenwickTree>(data_);
    t.stop();
    fen_built_ = true;
    return t.elapsed_ms();
}

double QueryExecutor::getBPlusTreeBuildTime() {
    Timer t;
    t.start();
    bplus_tree_ = std::make_unique<BPlusTree>();
    std::vector<BPlusTree::Record> records;
    records.reserve(data_.size());
    for (int64_t i = 0; i < (int64_t)data_.size(); i++) {
        records.push_back({i, data_[i]});
    }
    bplus_tree_->build(records);
    t.stop();
    bp_built_ = true;
    return t.elapsed_ms();
}

size_t QueryExecutor::getSegmentTreeMemory() const {
    return seg_built_ ? segment_tree_->memoryUsageBytes() : 0;
}

size_t QueryExecutor::getLazySegmentTreeMemory() const {
    return lazy_built_ ? lazy_segment_tree_->memoryUsageBytes() : 0;
}

size_t QueryExecutor::getFenwickTreeMemory() const {
    return fen_built_ ? fenwick_tree_->memoryUsageBytes() : 0;
}

size_t QueryExecutor::getBPlusTreeMemory() const {
    return bp_built_ ? bplus_tree_->memoryUsageBytes() : 0;
}

std::string QueryExecutor::getTrackedExecution(const std::string& sql) {
    ParsedQuery parsed = QueryParser::parse(sql);
    if (!parsed.valid) return "{\"error\":\"" + parsed.error_message + "\"}";
    
    if (!seg_built_) buildSegmentTree();
    
    int l = parsed.has_filter ? parsed.range_low : 0;
    int r = parsed.has_filter ? parsed.range_high : data_size_ - 1;
    
    auto visited = segment_tree_->trackedRangeSum(l, r);
    
    std::ostringstream oss;
    oss << "{\"query\":" << QueryParser::toJSON(parsed) << ",";
    oss << "\"tree\":" << segment_tree_->toJSON() << ",";
    oss << "\"visited\":[";
    for (size_t i = 0; i < visited.size(); i++) {
        if (i > 0) oss << ",";
        oss << "{\"node\":" << visited[i].node_idx
            << ",\"left\":" << visited[i].seg_left
            << ",\"right\":" << visited[i].seg_right
            << ",\"value\":" << visited[i].value
            << ",\"action\":\"" << visited[i].action << "\"}";
    }
    oss << "]}";
    return oss.str();
}

} // namespace querylens
