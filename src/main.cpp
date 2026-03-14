#include "../include/query/executor.hpp"
#include "../include/storage/dataset_generator.hpp"
#include "../include/utils/timer.hpp"
#include <iostream>
#include <string>
#include <sstream>

using namespace querylens;

void printBanner() {
    std::cout << R"(
  ╔═══════════════════════════════════════════════════════════╗
  ║                                                           ║
  ║     ██████╗ ██╗   ██╗███████╗██████╗ ██╗   ██╗            ║
  ║    ██╔═══██╗██║   ██║██╔════╝██╔══██╗╚██╗ ██╔╝            ║
  ║    ██║   ██║██║   ██║█████╗  ██████╔╝ ╚████╔╝             ║
  ║    ██║▄▄ ██║██║   ██║██╔══╝  ██╔══██╗  ╚██╔╝              ║
  ║    ╚██████╔╝╚██████╔╝███████╗██║  ██║   ██║               ║
  ║     ╚══▀▀═╝  ╚═════╝ ╚══════╝╚═╝  ╚═╝   ╚═╝               ║
  ║                                                           ║
  ║       ██╗     ███████╗███╗   ██╗███████╗                   ║
  ║       ██║     ██╔════╝████╗  ██║██╔════╝                   ║
  ║       ██║     █████╗  ██╔██╗ ██║███████╗                   ║
  ║       ██║     ██╔══╝  ██║╚██╗██║╚════██║                   ║
  ║       ███████╗███████╗██║ ╚████║███████║                   ║
  ║       ╚══════╝╚══════╝╚═╝  ╚═══╝╚══════╝                   ║
  ║                                                           ║
  ║  Accelerating Aggregate Queries with Segment Trees        ║
  ║  Research-Grade Analytical Query Engine                    ║
  ╚═══════════════════════════════════════════════════════════╝
)" << std::endl;
}

void printHelp() {
    std::cout << "\nCommands:\n"
              << "  load <csv_file>     - Load a dataset from CSV\n"
              << "  generate            - Generate benchmark datasets (1k, 10k, 100k, 1M)\n"
              << "  build               - Build all index structures\n"
              << "  query <SQL>         - Execute a query (auto-selects best method)\n"
              << "  scan <SQL>          - Execute with sequential scan\n"
              << "  segtree <SQL>       - Execute with segment tree\n"
              << "  fenwick <SQL>       - Execute with Fenwick tree\n"
              << "  bplus <SQL>         - Execute with B+ tree\n"
              << "  compare <SQL>       - Compare all methods on a query\n"
              << "  info                - Show loaded dataset info\n"
              << "  help                - Show this help\n"
              << "  exit                - Exit QueryLens\n"
              << "\nExample queries:\n"
              << "  SELECT SUM(sales) WHERE day BETWEEN 10 AND 100\n"
              << "  SELECT MIN(sales) WHERE day BETWEEN 50 AND 500\n"
              << "  SELECT MAX(sales) WHERE day BETWEEN 100 AND 9999\n"
              << "  SELECT AVG(sales) WHERE day BETWEEN 0 AND 999\n"
              << "  SELECT COUNT(sales) WHERE day BETWEEN 200 AND 800\n"
              << std::endl;
}

int main(int argc, char* argv[]) {
    printBanner();
    
    QueryExecutor executor;
    bool data_loaded = false;
    
    std::cout << "Type 'help' for available commands.\n" << std::endl;
    
    std::string line;
    while (true) {
        std::cout << "\033[1;36mQueryLens>\033[0m ";
        if (!std::getline(std::cin, line)) break;
        
        // Trim
        size_t start = line.find_first_not_of(" \t");
        if (start == std::string::npos) continue;
        line = line.substr(start);
        
        if (line.empty()) continue;
        
        if (line == "exit" || line == "quit") {
            std::cout << "Goodbye!\n";
            break;
        }
        
        if (line == "help") {
            printHelp();
            continue;
        }
        
        if (line == "generate") {
            DatasetGenerator::generateBenchmarkSets("data");
            std::cout << "Benchmark datasets generated in data/ directory.\n";
            continue;
        }
        
        if (line.substr(0, 4) == "load") {
            std::string filename = line.length() > 5 ? line.substr(5) : "";
            if (filename.empty()) {
                std::cout << "Usage: load <filename.csv>\n";
                continue;
            }
            // Trim filename
            size_t fstart = filename.find_first_not_of(" \t");
            if (fstart != std::string::npos) filename = filename.substr(fstart);
            
            std::cout << "Loading " << filename << "...\n";
            executor.loadData(filename);
            data_loaded = executor.dataSize() > 0;
            if (data_loaded) {
                std::cout << "Loaded " << executor.dataSize() << " records.\n";
                std::cout << "Building indexes...\n";
                
                Timer t;
                t.start();
                executor.buildAllIndexes();
                t.stop();
                std::cout << "All indexes built in " << t.elapsed_ms() << " ms.\n";
            } else {
                std::cout << "Failed to load data from " << filename << "\n";
            }
            continue;
        }
        
        if (line == "build") {
            if (!data_loaded) {
                std::cout << "No data loaded. Use 'load <file>' first.\n";
                continue;
            }
            std::cout << "Building indexes...\n";
            std::cout << "  Segment Tree:      " << executor.getSegmentTreeBuildTime() << " ms\n";
            std::cout << "  Lazy Segment Tree: " << executor.getLazySegmentTreeBuildTime() << " ms\n";
            std::cout << "  Fenwick Tree:      " << executor.getFenwickTreeBuildTime() << " ms\n";
            std::cout << "  B+ Tree:           " << executor.getBPlusTreeBuildTime() << " ms\n";
            continue;
        }
        
        if (line == "info") {
            if (!data_loaded) {
                std::cout << "No data loaded.\n";
            } else {
                std::cout << "Dataset size: " << executor.dataSize() << " records\n";
                std::cout << "Memory usage:\n";
                std::cout << "  Segment Tree:      " << executor.getSegmentTreeMemory() / 1024.0 << " KB\n";
                std::cout << "  Lazy Segment Tree: " << executor.getLazySegmentTreeMemory() / 1024.0 << " KB\n";
                std::cout << "  Fenwick Tree:      " << executor.getFenwickTreeMemory() / 1024.0 << " KB\n";
                std::cout << "  B+ Tree:           " << executor.getBPlusTreeMemory() / 1024.0 << " KB\n";
            }
            continue;
        }
        
        // Query commands
        if (!data_loaded && (line.substr(0, 5) == "query" || line.substr(0, 4) == "scan" ||
            line.substr(0, 7) == "segtree" || line.substr(0, 7) == "fenwick" ||
            line.substr(0, 5) == "bplus" || line.substr(0, 7) == "compare")) {
            std::cout << "No data loaded. Use 'load <file>' first.\n";
            continue;
        }
        
        if (line.substr(0, 7) == "compare") {
            std::string sql = line.length() > 8 ? line.substr(8) : "";
            if (sql.empty()) {
                std::cout << "Usage: compare <SQL query>\n";
                continue;
            }
            
            std::cout << "\n┌─────────────────────┬──────────────┬──────────────┐\n";
            std::cout << "│ Method              │ Result       │ Time (ms)    │\n";
            std::cout << "├─────────────────────┼──────────────┼──────────────┤\n";
            
            ExecutionMethod methods[] = {
                ExecutionMethod::SEQUENTIAL_SCAN,
                ExecutionMethod::SEGMENT_TREE,
                ExecutionMethod::LAZY_SEGMENT_TREE,
                ExecutionMethod::FENWICK_TREE,
                ExecutionMethod::BPLUS_TREE
            };
            
            for (auto m : methods) {
                auto result = executor.executeWith(sql, m);
                printf("│ %-19s │ %12.0f │ %12.4f │\n",
                       result.method_name.c_str(), result.result_value, result.execution_time_ms);
            }
            std::cout << "└─────────────────────┴──────────────┴──────────────┘\n";
            continue;
        }
        
        // Specific method queries
        ExecutionMethod forced_method;
        std::string sql;
        bool use_forced = false;
        
        if (line.substr(0, 4) == "scan") {
            sql = line.length() > 5 ? line.substr(5) : "";
            forced_method = ExecutionMethod::SEQUENTIAL_SCAN;
            use_forced = true;
        } else if (line.substr(0, 7) == "segtree") {
            sql = line.length() > 8 ? line.substr(8) : "";
            forced_method = ExecutionMethod::SEGMENT_TREE;
            use_forced = true;
        } else if (line.substr(0, 7) == "fenwick") {
            sql = line.length() > 8 ? line.substr(8) : "";
            forced_method = ExecutionMethod::FENWICK_TREE;
            use_forced = true;
        } else if (line.substr(0, 5) == "bplus") {
            sql = line.length() > 6 ? line.substr(6) : "";
            forced_method = ExecutionMethod::BPLUS_TREE;
            use_forced = true;
        } else if (line.substr(0, 5) == "query") {
            sql = line.length() > 6 ? line.substr(6) : "";
        } else if (line.substr(0, 6) == "SELECT" || line.substr(0, 6) == "select") {
            sql = line; // Direct SQL input
        } else {
            std::cout << "Unknown command. Type 'help' for available commands.\n";
            continue;
        }
        
        if (sql.empty()) {
            std::cout << "Please provide a SQL query.\n";
            continue;
        }
        
        QueryResult result;
        if (use_forced) {
            result = executor.executeWith(sql, forced_method);
        } else {
            result = executor.execute(sql);
        }
        
        if (result.success) {
            std::cout << "\n  Method: " << result.method_name << "\n";
            std::cout << "  Result: " << result.result_value << "\n";
            std::cout << "  Time:   " << result.execution_time_ms << " ms\n\n";
        } else {
            std::cout << "  Error: " << result.error_message << "\n";
        }
    }
    
    return 0;
}
