#ifndef QUERYLENS_MEMORY_TRACKER_HPP
#define QUERYLENS_MEMORY_TRACKER_HPP

#include <cstddef>
#include <fstream>
#include <string>
#include <sstream>

namespace querylens {

class MemoryTracker {
public:
    // Returns current resident set size (physical memory used) in KB
    static size_t getCurrentRSSKB() {
        std::ifstream statm("/proc/self/statm");
        if (!statm.is_open()) return 0;
        
        size_t size, resident;
        statm >> size >> resident;
        // resident is in pages, convert to KB (page size = 4096 bytes typically)
        return resident * 4; // 4096 / 1024 = 4
    }

    // Returns current RSS in bytes
    static size_t getCurrentRSSBytes() {
        return getCurrentRSSKB() * 1024;
    }

    // Returns current RSS in MB
    static double getCurrentRSSMB() {
        return getCurrentRSSKB() / 1024.0;
    }

    // Simple memory snapshot utility
    struct Snapshot {
        size_t rss_kb;
        double rss_mb;
    };

    static Snapshot takeSnapshot() {
        size_t kb = getCurrentRSSKB();
        return {kb, kb / 1024.0};
    }

    // Compute difference between two snapshots
    static double diffMB(const Snapshot& before, const Snapshot& after) {
        if (after.rss_kb >= before.rss_kb)
            return (after.rss_kb - before.rss_kb) / 1024.0;
        return 0.0;
    }
};

} // namespace querylens

#endif // QUERYLENS_MEMORY_TRACKER_HPP
