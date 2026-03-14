#ifndef QUERYLENS_PARSER_HPP
#define QUERYLENS_PARSER_HPP

#include <string>
#include <vector>
#include <cstdint>

namespace querylens {

// Supported aggregate functions
enum class AggFunc { SUM, MIN, MAX, AVG, COUNT };

// Parsed query representation
struct ParsedQuery {
    AggFunc aggregate;           // Aggregate function
    std::string column;          // Column to aggregate (e.g., "sales")
    std::string filter_column;   // Column in WHERE clause (e.g., "day")
    int64_t range_low;           // Lower bound of range
    int64_t range_high;          // Upper bound of range
    bool has_filter;             // Whether a WHERE clause is present
    bool valid;                  // Whether parsing succeeded
    std::string error_message;   // Error message if parsing failed
    std::string raw_query;       // Original query string
};

// SQL-like query parser
// Supports:
//   SELECT SUM(sales) WHERE day BETWEEN 10 AND 100
//   SELECT MIN(sales) WHERE day BETWEEN 50 AND 500
//   SELECT MAX(sales)
//   SELECT AVG(sales) WHERE day BETWEEN 1 AND 1000
//   SELECT COUNT(sales) WHERE day BETWEEN 10 AND 200
class QueryParser {
public:
    static ParsedQuery parse(const std::string& query);
    
    // Convert AggFunc to string
    static std::string aggFuncToString(AggFunc func);
    
    // Convert ParsedQuery to JSON string for frontend
    static std::string toJSON(const ParsedQuery& query);

private:
    static std::string trim(const std::string& str);
    static std::string toUpper(const std::string& str);
};

} // namespace querylens

#endif // QUERYLENS_PARSER_HPP
