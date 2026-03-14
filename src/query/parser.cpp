#include "../../include/query/parser.hpp"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>

namespace querylens {

std::string QueryParser::trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = str.find_last_not_of(" \t\r\n");
    return str.substr(start, end - start + 1);
}

std::string QueryParser::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

std::string QueryParser::aggFuncToString(AggFunc func) {
    switch (func) {
        case AggFunc::SUM:   return "SUM";
        case AggFunc::MIN:   return "MIN";
        case AggFunc::MAX:   return "MAX";
        case AggFunc::AVG:   return "AVG";
        case AggFunc::COUNT: return "COUNT";
        default:             return "UNKNOWN";
    }
}

ParsedQuery QueryParser::parse(const std::string& query) {
    ParsedQuery result;
    result.raw_query = query;
    result.valid = false;
    result.has_filter = false;
    result.range_low = 0;
    result.range_high = 0;
    
    std::string q = trim(query);
    if (q.empty()) {
        result.error_message = "Empty query";
        return result;
    }
    
    std::string upper_q = toUpper(q);
    
    // Must start with SELECT
    if (upper_q.substr(0, 6) != "SELECT") {
        result.error_message = "Query must start with SELECT";
        return result;
    }
    
    // Extract aggregate function and column
    // Pattern: SELECT AGG(column) [WHERE filter_col BETWEEN low AND high]
    
    // Find the aggregate function
    size_t agg_start = upper_q.find_first_of("SMCMA", 7);
    
    // Try each aggregate function
    std::string agg_part;
    if (upper_q.find("SUM(", 6) != std::string::npos) {
        result.aggregate = AggFunc::SUM;
        agg_start = upper_q.find("SUM(", 6);
        agg_part = q.substr(agg_start);
    } else if (upper_q.find("MIN(", 6) != std::string::npos) {
        result.aggregate = AggFunc::MIN;
        agg_start = upper_q.find("MIN(", 6);
        agg_part = q.substr(agg_start);
    } else if (upper_q.find("MAX(", 6) != std::string::npos) {
        result.aggregate = AggFunc::MAX;
        agg_start = upper_q.find("MAX(", 6);
        agg_part = q.substr(agg_start);
    } else if (upper_q.find("AVG(", 6) != std::string::npos) {
        result.aggregate = AggFunc::AVG;
        agg_start = upper_q.find("AVG(", 6);
        agg_part = q.substr(agg_start);
    } else if (upper_q.find("COUNT(", 6) != std::string::npos) {
        result.aggregate = AggFunc::COUNT;
        agg_start = upper_q.find("COUNT(", 6);
        agg_part = q.substr(agg_start);
    } else {
        result.error_message = "No valid aggregate function found (SUM, MIN, MAX, AVG, COUNT)";
        return result;
    }
    
    // Extract column name from between parentheses
    size_t paren_open = agg_part.find('(');
    size_t paren_close = agg_part.find(')');
    if (paren_open == std::string::npos || paren_close == std::string::npos) {
        result.error_message = "Malformed aggregate function (missing parentheses)";
        return result;
    }
    result.column = trim(agg_part.substr(paren_open + 1, paren_close - paren_open - 1));
    
    // Check for WHERE clause
    std::string after_agg = toUpper(q.substr(agg_start + paren_close + 1));
    after_agg = trim(after_agg);
    
    if (!after_agg.empty()) {
        size_t where_pos = after_agg.find("WHERE");
        if (where_pos == std::string::npos) {
            result.error_message = "Expected WHERE clause after aggregate function";
            return result;
        }
        
        std::string where_clause = trim(after_agg.substr(where_pos + 5));
        
        // Parse: filter_col BETWEEN low AND high
        size_t between_pos = where_clause.find("BETWEEN");
        if (between_pos == std::string::npos) {
            result.error_message = "WHERE clause must use BETWEEN ... AND ...";
            return result;
        }
        
        // Get filter column
        result.filter_column = trim(where_clause.substr(0, between_pos));
        // Convert to lowercase for matching
        std::transform(result.filter_column.begin(), result.filter_column.end(),
                       result.filter_column.begin(), ::tolower);
        
        // Get range values
        std::string range_part = trim(where_clause.substr(between_pos + 7));
        size_t and_pos = range_part.find("AND");
        if (and_pos == std::string::npos) {
            result.error_message = "BETWEEN clause must include AND";
            return result;
        }
        
        std::string low_str = trim(range_part.substr(0, and_pos));
        std::string high_str = trim(range_part.substr(and_pos + 3));
        
        try {
            result.range_low = std::stoll(low_str);
            result.range_high = std::stoll(high_str);
        } catch (...) {
            result.error_message = "Invalid numeric values in BETWEEN clause";
            return result;
        }
        
        result.has_filter = true;
    }
    
    result.valid = true;
    return result;
}

std::string QueryParser::toJSON(const ParsedQuery& query) {
    std::ostringstream oss;
    oss << "{";
    oss << "\"valid\":" << (query.valid ? "true" : "false") << ",";
    oss << "\"aggregate\":\"" << aggFuncToString(query.aggregate) << "\",";
    oss << "\"column\":\"" << query.column << "\",";
    oss << "\"has_filter\":" << (query.has_filter ? "true" : "false") << ",";
    if (query.has_filter) {
        oss << "\"filter_column\":\"" << query.filter_column << "\",";
        oss << "\"range_low\":" << query.range_low << ",";
        oss << "\"range_high\":" << query.range_high << ",";
    }
    oss << "\"raw_query\":\"" << query.raw_query << "\"";
    if (!query.error_message.empty()) {
        oss << ",\"error\":\"" << query.error_message << "\"";
    }
    oss << "}";
    return oss.str();
}

} // namespace querylens
