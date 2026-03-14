<div align="center">
  <img src="https://img.shields.io/badge/QueryLens-1E3A8A?style=for-the-badge&logoColor=white" alt="QueryLens Logo" height="60" />
</div>

<h1 align="center">🔍 QueryLens: High-Performance Analytical Engine</h1>

<div align="center">
  
![Status](https://img.shields.io/badge/Status-Active-success?style=for-the-badge) 
![C++](https://img.shields.io/badge/C++-17-00599C?style=for-the-badge&logo=c%2B%2B) 
![License](https://img.shields.io/badge/License-MIT-green?style=for-the-badge) 
![D3.js](https://img.shields.io/badge/D3.js-Frontend-F9A03C?style=for-the-badge&logo=d3.js)

<p align="center">
  <b>A research-grade, highly optimized C++17 analytical database engine implementing <br/> Segment Trees, Lazy Propagation, and Advanced Tree Structures.</b>
</p>

</div>

---

## ⚡ Overview

As data volumes scale exponentially, traditional database filtering and aggregation approaches (like Sequential Scans) hit severe processing bottlenecks, requiring $\mathcal{O}(N)$ compute time. **QueryLens** explores, implements, and empirically benchmarks advanced multidimensional data structures designed to drastically accelerate range aggregate queries down to strict logarithmic $\mathcal{O}(\log N)$ boundaries.

Alongside the C++ backend execution engine, QueryLens ships with an interactive, animated **D3.js Visual Dashboard** to demystify how these complex indexing algorithms function natively.

---

## 📖 Table of Contents
1. [Core Features & Objectives](#-core-features--objectives)
2. [Data Structures & Complexities](#-data-structures--complexities)
3. [The Query Processing Pipeline](#-the-query-processing-pipeline)
4. [Empirical Benchmarks](#-empirical-benchmarks)
5. [Interactive D3.js Workspace](#-interactive-d3js-workspace)

---

## 🚀 Core Features & Objectives

* ⚡ **High-Performance Computing Engine**: Native C++ implementations avoiding dynamic memory thrashing.
* 🧠 **Automated Query Planning**: A custom SQL parser that converts string commands dynamically into AST execution plans.
* 🌳 **Robust Algorithms**: Benchmarking **Segment Trees**, **Lazy Propagation**, **Fenwick Trees**, and **B+ Trees**.
* 🛡 **Jitter-Protected Benchmarking**: Built-in Monte-Carlo evaluation suits tracing pure $\mathcal{O}(\log N)$ scaling up to 1,000,000 rows without cache artifacts.
* 📊 **Web Analytics Visualization**: Translating raw backend JSON formats directly into graphical SVG overlays dynamically.

---

## 🧱 Data Structures & Complexities

QueryLens meticulously maps different data structures to solve the *Range Aggregation Problem*.

### 1. Sequential Scan (`O(N)`)
The standard control baseline. It linearly iterates through memory from row `start` to `end`. While highly cache-friendly for tiny localized clusters, it becomes fatally slow as dataset sizes approach $10^6+$.

### 2. Segment Tree (`O(log N)`)
Implemented implicitly using a 1D `std::vector` of size $4N$.
* **Supported Aggregations**: `SUM`, `MIN`, `MAX`, `AVG`, `COUNT`
* **Build Time**: $\mathcal{O}(N)$
* **Query Time**: $\mathcal{O}(\log N)$
* **How it works:** Nodes represent cached sums of their children. Whenever a query range perfectly encapsulates an internal node's tracking range, it returns the value instantly, bypassing millions of redundant leaf computations.

### 3. Lazy Segment Tree (`O(log N)`)
When modifying massive consecutive ranges (e.g., `UPDATE sales SET value = value + 10 WHERE day BETWEEN 10 AND 50000`), a standard Segment Tree falls back to $\mathcal{O}(N \log N)$ by updating every single leaf. 
* **Lazy Propagation**: We suspend physical leaf updates. Instead, we flag parent nodes with an internal `lazy[]` cache array. These tagged values are pushed downward to children *only* when that specific branch is queried again.
* **Range Update Time**: Strictly bounded to $\mathcal{O}(\log N)$.

### 4. Fenwick Tree / Binary Indexed Tree (`O(log N)`)
A bit-manipulation marvel that computes Prefix Sums utilizing minimal memory footprints (exactly $N$ size arrays compared to Segment Tree's $4N$). 
* **Limitation**: While brilliant for `SUM` operations, it fundamentally lacks the capability to cleanly return disjoint `MIN`/`MAX` bounding threshold metrics without breaking bounds.
* **Traversal**: Computes intervals rapidly using `index & (-index)` operations natively mapping integers directly.

### 5. B+ Tree (`O(log N + K)`)
Designed heavily for disk-based clustered indices indexing external memories. While Segment Trees accelerate *aggregations*, the B+ Tree excels at *filtering*. 
* **Methodology**: We implement a linked-list at the leaf layer allowing rapid traversal. It scans tree depth to find the low-bound $\mathcal{O}(\log N)$, then linearly navigates $K$ matching linked leaves.

---

## 🚂 The Query Processing Pipeline

QueryLens simulates real-world database ecosystems using an ad-hoc SQL console interface:

```sql
QueryLens> SELECT SUM(sales) WHERE day BETWEEN 1000 AND 500000
```

1. **Parser Tokenization**: Validates syntax via regex. Identifies `SUM` as the AggFunc, `sales` as the target column, and parses constants `[1000, 500000]`.
2. **Execution Planner**: Scans available data structures. Recognizing the constraint `SUM`, it leverages rulesets to bypass the robust B+ tree (slower sequential $K$) and forces engine routing to `ExecutionMethod::SEGMENT_TREE`.
3. **Execution Tracker**: Spans exact tree recursion matrices mapping the bounds. Collects sub-nanosecond precision timing natively testing bounds formatting outputs globally to JSON.

---

## 📈 Empirical Benchmarks

QueryLens is extensively benchmarked using automated testing harnesses. 

- **Datasets Generated**: Scaling distributions tracing 1k, 10k, 100k, and **1 Million** records.
- **Jitter Protection**: To prevent CPU-cache hits artificially inflating the response metrics on repeating arrays, we inject random boundary jitter (e.g., testing `[25%+3, 75%-2]` consecutively) forcing fresh algorithmic executions.

#### `SUM()` Query Execution Latency Analysis

| Dataset Size | Seq. Scan `O(N)` | Segment Tree `O(log N)` | B+ Tree | Fenwick Tree |
| :--- | :--- | :--- | :--- | :--- |
| **1,000** | 0.0001 ms | 0.0001 ms | 0.0010 ms | < 0.0001 ms |
| **10,000** | 0.0010 ms | < 0.0001 ms | 0.0111 ms | < 0.0001 ms |
| **100,000** | 0.0121 ms | 0.0000 ms| 0.1070 ms | < 0.0001 ms |
| **1,000,000**| **0.1281 ms** | **0.0008 ms** | 1.4522 ms | < 0.0001 ms |

> 🏆 **Conclusion:** Notice the explicit $\mathcal{O}(N)$ spiral of the Sequential Scan rising linearly to `0.1281 ms` at 1M records, while the Segment Tree entirely flattens standard execution, computing the response in under a microsecond (`0.0008 ms`). **This results in an empirically proven 160x hardware speedup!**

---

## 🎨 Interactive D3.js Workspace

QueryLens is bundled with a completely uncoupled, browser-native Dashboard explicitly animating background configurations!

#### Dashboard Environments:
1. **Interactive Query Console System**: A mocked terminal that parses inputs to map onto a graphical dataset visualizer mapping distributions visually via real-time highlights!
2. **Segment Tree Path Tracer**: Generates SVG paths traversing down mock tree dependencies. 
   - *Yellow Links*: Partial overlap, pushing recursion lower.
   - *Green Nodes*: Full condition satisfied, perfectly bounded overlaps.
3. **Lazy Propagator Animation**: Visual circles depicting recursive "Push-Downs" dropping cached parent modifications onto children domains asynchronously.
4. **Log-Log Scaled Performance Overlays**: Auto-rendering benchmark empirical curves tracing visual proofs. 

---

## 💻 Comprehensive Compilation & Usage Guide

### Dependencies & Requirements
- **Modern C++ Compiler**: `g++ 9.0+` or `clang++` (requires C++17 support).
- **GNU Make Utility**: `make` (for automated build mapping).
- **Python 3.x**: Used exclusively to spawn the frontend mock HTML server (`python3`).

### 1. Engine Compilation
Navigate to the root repository directory and utilize the `Makefile` to trigger the compilation pipelines cleanly producing our backend executable targets.

```bash
$ cd QueryLens
$ make clean
$ make all
```
*This binds component linkages directly out of the `src/` hierarchy and yields two central binaries locally:*
- `querylens`: The Interactive SQL query REPL terminal.
- `benchmark`: The automated scalable multi-threaded hardware evaluation suite.

### 2. Generating Datasets & Running Benchmarks
To immediately calculate $\mathcal{O}(N)$ sequential bottlenecks against algorithmic logarithmic performance natively, generate the test environments running this specific command:
```bash
$ make data
```
*(This triggers the `benchmark_main.cpp` routine, physically spawning distributions from 1,000 to precisely 1,000,000 bounds mapped gracefully. Results format dynamic timing components globally dropping arrays securely into `results/benchmark_results.json` for frontend evaluation.)*

### 3. Launching the Interactive Query REPL
Engage with the system interactively formatting limits logically tracking parsing structs:
```bash
$ ./querylens
```
Use the live server prompt to securely benchmark explicit SQL queries:
```sql
QueryLens> load data/dataset_1m.csv
QueryLens> query SELECT SUM(sales) WHERE day BETWEEN 1000 AND 950000
QueryLens> compare SELECT AVG(sales) WHERE day BETWEEN 1000 AND 950000
QueryLens> scan SELECT MAX(sales) WHERE day BETWEEN 50000 AND 900000
```
*Type `exit` or `quit` to cleanly evaluate evaluations gracefully killing constraints.*

### 4. Running the Web Frontend

Initialize a basic local HTTP server using Python determining configurations correctly accessing D3 dependencies defining topologies securely:
```bash
$ cd frontend/
$ python3 -m http.server 8000
```
Open a modern browser instance (Chrome, Firefox) and navigate to **[http://localhost:8000](http://localhost:8000)** to safely evaluate graphic layouts dependably verifying boundaries natively identifying layouts flawlessly defining logics.

---

## 📁 Repository Structure

```text
QueryLens/
├── Makefile                    # Target compilation routines & linkers
├── README.md                   # You are here!
├── include/                    # Header Class specifications
│   ├── benchmark/              # Jitter scaling logic frameworks
│   ├── index/                  # Abstract setups for SegTree, B+Tree, Fenwick
│   ├── query/                  # SQL AST Parser mapping & Executor Routing
│   └── storage/                # CSV I/O parsing streams & randomizations
├── src/                        # Implementations corresponding to include/
│   ├── benchmark_main.cpp      # Isolated analytical scaling entrypoint
│   └── main.cpp                # Interactive Core REPL environment
└── frontend/                   # Interactive Dashboard Assets
    ├── index.html              # HTML Root
    ├── css/style.css           # Themes & DOM layouts
    └── js/                     
        ├── app.js              # Event-listener binders
        └── *_viz.js            # Extensible SVG Tree Animation APIs
```

---

*Architected & Optimized for Advanced Data Structure Analysis / Analytical Query Demonstrations.*
