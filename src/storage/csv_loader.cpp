#include "../../include/storage/csv_loader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

namespace querylens {

std::vector<DataRecord> CSVLoader::load(const std::string& filename) {
    std::vector<DataRecord> records;
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "[CSVLoader] Error: Could not open file: " << filename << std::endl;
        return records;
    }
    
    std::string line;
    bool first_line = true;
    
    while (std::getline(file, line)) {
        // Skip empty lines
        if (line.empty()) continue;
        
        // Skip header line if it contains non-numeric characters
        if (first_line) {
            first_line = false;
            // Check if first character is a letter (header row)
            std::string trimmed = line;
            trimmed.erase(0, trimmed.find_first_not_of(" \t\r\n"));
            if (!trimmed.empty() && !std::isdigit(trimmed[0]) && trimmed[0] != '-') {
                continue; // Skip header
            }
        }
        
        std::stringstream ss(line);
        std::string token;
        DataRecord rec;
        
        // Parse: key,value
        if (std::getline(ss, token, ',')) {
            try {
                rec.key = std::stoll(token);
            } catch (...) { continue; }
        } else continue;
        
        if (std::getline(ss, token, ',')) {
            try {
                rec.value = std::stoll(token);
            } catch (...) { continue; }
        } else continue;
        
        records.push_back(rec);
    }
    
    file.close();
    return records;
}

std::vector<int64_t> CSVLoader::loadValues(const std::string& filename) {
    auto records = load(filename);
    std::vector<int64_t> values;
    values.reserve(records.size());
    for (const auto& rec : records) {
        values.push_back(rec.value);
    }
    return values;
}

void CSVLoader::save(const std::string& filename, const std::vector<DataRecord>& records) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "[CSVLoader] Error: Could not write to file: " << filename << std::endl;
        return;
    }
    
    file << "day,sales\n";
    for (const auto& rec : records) {
        file << rec.key << "," << rec.value << "\n";
    }
    file.close();
}

} // namespace querylens
