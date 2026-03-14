CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra -I include
LDFLAGS = -lm

# Source files
INDEX_SRCS = src/index/segment_tree.cpp src/index/lazy_segment_tree.cpp \
             src/index/fenwick_tree.cpp src/index/bplus_tree.cpp
STORAGE_SRCS = src/storage/csv_loader.cpp src/storage/dataset_generator.cpp
QUERY_SRCS = src/query/parser.cpp src/query/planner.cpp src/query/executor.cpp
BENCH_SRCS = src/benchmark/metrics.cpp src/benchmark/benchmark_runner.cpp

ALL_SRCS = $(INDEX_SRCS) $(STORAGE_SRCS) $(QUERY_SRCS) $(BENCH_SRCS)

# Object files
OBJDIR = build
ALL_OBJS = $(patsubst src/%.cpp,$(OBJDIR)/%.o,$(ALL_SRCS))

# Targets
QUERYLENS = querylens
BENCHMARK = benchmark

.PHONY: all clean run benchmark data help

all: $(QUERYLENS) $(BENCHMARK)

# QueryLens interactive CLI
$(QUERYLENS): $(ALL_OBJS) $(OBJDIR)/main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Benchmark runner
$(BENCHMARK): $(ALL_OBJS) $(OBJDIR)/benchmark_main.o
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

# Compile source files
$(OBJDIR)/%.o: src/%.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/main.o: src/main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJDIR)/benchmark_main.o: src/benchmark/benchmark_main.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Generate benchmark datasets
data:
	@mkdir -p data
	./$(BENCHMARK) data results

# Run interactive QueryLens
run: $(QUERYLENS)
	./$(QUERYLENS)

# Run benchmarks
run-benchmark: $(BENCHMARK)
	@mkdir -p data results
	./$(BENCHMARK) data results

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(QUERYLENS) $(BENCHMARK)

help:
	@echo "QueryLens Makefile"
	@echo ""
	@echo "Targets:"
	@echo "  all           - Build both querylens and benchmark"
	@echo "  querylens     - Build the interactive query engine"
	@echo "  benchmark     - Build the benchmark runner"
	@echo "  run           - Run the interactive query engine"
	@echo "  run-benchmark - Generate data and run full benchmark"
	@echo "  data          - Generate benchmark datasets"
	@echo "  clean         - Remove build artifacts"
	@echo "  help          - Show this help"
