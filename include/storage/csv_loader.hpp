#ifndef QUERYLENS_CSV_LOADER_HPP
#define QUERYLENS_CSV_LOADER_HPP

#include <vector>
#include <string>
#include <cstdint>

namespace querylens {

struct DataRecord {
    int64_t key;    // Primary key / index column (e.g., day)
    int64_t value;  // Aggregate column (e.g., sales)
};

class CSVLoader {
public:
    // Load CSV with two columns: key, value
    static std::vector<DataRecord> load(const std::string& filename);
    
    // Load only the value column (for segment tree / fenwick tree building)
    static std::vector<int64_t> loadValues(const std::string& filename);
    
    // Save records to CSV
    static void save(const std::string& filename, const std::vector<DataRecord>& records);
};

} // namespace querylens

#endif // QUERYLENS_CSV_LOADER_HPP
