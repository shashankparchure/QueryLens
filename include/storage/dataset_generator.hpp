#ifndef QUERYLENS_DATASET_GENERATOR_HPP
#define QUERYLENS_DATASET_GENERATOR_HPP

#include <string>
#include <cstdint>

namespace querylens {

class DatasetGenerator {
public:
    enum class Distribution {
        UNIFORM,        // Uniform random values
        NORMAL,         // Normal distribution
        SKEWED,         // Power-law / skewed distribution
        TIMESERIES      // Simulated time-series with trend
    };

    // Generate dataset with N rows, values in [min_val, max_val]
    static void generate(const std::string& filename, int64_t num_rows,
                         int64_t min_val = 1, int64_t max_val = 10000,
                         Distribution dist = Distribution::UNIFORM,
                         unsigned seed = 42);

    // Generate all benchmark datasets (1k, 10k, 100k, 1M)
    static void generateBenchmarkSets(const std::string& data_dir);
};

} // namespace querylens

#endif // QUERYLENS_DATASET_GENERATOR_HPP
