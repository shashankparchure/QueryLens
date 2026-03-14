#include "../../include/storage/dataset_generator.hpp"
#include "../../include/storage/csv_loader.hpp"
#include <fstream>
#include <random>
#include <cmath>
#include <iostream>
#include <sys/stat.h>

namespace querylens {

void DatasetGenerator::generate(const std::string& filename, int64_t num_rows,
                                 int64_t min_val, int64_t max_val,
                                 Distribution dist, unsigned seed) {
    std::mt19937_64 rng(seed);
    std::vector<DataRecord> records;
    records.reserve(num_rows);
    
    for (int64_t i = 0; i < num_rows; i++) {
        DataRecord rec;
        rec.key = i; // day / index
        
        switch (dist) {
            case Distribution::UNIFORM: {
                std::uniform_int_distribution<int64_t> uniform(min_val, max_val);
                rec.value = uniform(rng);
                break;
            }
            case Distribution::NORMAL: {
                double mean = (min_val + max_val) / 2.0;
                double stddev = (max_val - min_val) / 6.0;
                std::normal_distribution<double> normal(mean, stddev);
                rec.value = std::max(min_val, std::min(max_val, (int64_t)std::round(normal(rng))));
                break;
            }
            case Distribution::SKEWED: {
                // Power-law distribution
                std::uniform_real_distribution<double> uni(0.0, 1.0);
                double alpha = 2.0;
                double u = uni(rng);
                double val = min_val + (max_val - min_val) * std::pow(u, 1.0 / alpha);
                rec.value = (int64_t)std::round(val);
                break;
            }
            case Distribution::TIMESERIES: {
                // Simulated time-series with trend + noise
                double trend = min_val + (max_val - min_val) * ((double)i / num_rows);
                std::normal_distribution<double> noise(0, (max_val - min_val) * 0.1);
                double seasonal = (max_val - min_val) * 0.15 * std::sin(2.0 * M_PI * i / 365.0);
                rec.value = std::max(min_val, std::min(max_val,
                    (int64_t)std::round(trend + seasonal + noise(rng))));
                break;
            }
        }
        
        records.push_back(rec);
    }
    
    CSVLoader::save(filename, records);
    std::cout << "[DatasetGenerator] Generated " << num_rows << " rows -> " << filename << std::endl;
}

void DatasetGenerator::generateBenchmarkSets(const std::string& data_dir) {
    // Create data directory if it doesn't exist
    mkdir(data_dir.c_str(), 0755);
    
    struct {
        std::string name;
        int64_t rows;
    } datasets[] = {
        {"dataset_1k.csv",    1000},
        {"dataset_10k.csv",   10000},
        {"dataset_100k.csv",  100000},
        {"dataset_1m.csv",    1000000}
    };
    
    for (const auto& ds : datasets) {
        std::string path = data_dir + "/" + ds.name;
        generate(path, ds.rows, 1, 10000, Distribution::UNIFORM, 42);
    }
    
    std::cout << "[DatasetGenerator] All benchmark datasets generated in " << data_dir << std::endl;
}

} // namespace querylens
