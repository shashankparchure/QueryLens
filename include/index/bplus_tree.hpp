#ifndef QUERYLENS_BPLUS_TREE_HPP
#define QUERYLENS_BPLUS_TREE_HPP

#include <vector>
#include <cstdint>
#include <string>

namespace querylens {

// B+ Tree Index for range filtering and ordered traversal
// Leaf nodes are linked for efficient range scans
class BPlusTree {
public:
    static const int ORDER = 64; // B+ tree order (max children per node)

    struct Record {
        int64_t key;      // Index key (e.g., day number)
        int64_t value;    // Value (e.g., sales amount)
    };

private:
    struct Node {
        bool is_leaf;
        std::vector<int64_t> keys;
        std::vector<Node*> children;  // Internal nodes: child pointers
        std::vector<Record> records;  // Leaf nodes: records
        Node* next;                   // Leaf linked list
        
        Node(bool leaf) : is_leaf(leaf), next(nullptr) {}
    };

    Node* root_;
    int size_;

    void insertInternal(int64_t key, Node* cursor, Node* child);
    Node* findLeaf(int64_t key) const;
    void splitLeaf(Node* leaf);
    void splitInternal(Node* node);
    void destroyTree(Node* node);

public:
    BPlusTree();
    ~BPlusTree();

    void build(const std::vector<Record>& records);
    void insert(int64_t key, int64_t value);
    
    // Range query: collect all values where key is in [low, high]
    std::vector<int64_t> rangeSearch(int64_t low, int64_t high) const;
    
    // Aggregate queries over range
    int64_t rangeSum(int64_t low, int64_t high) const;
    int64_t rangeMin(int64_t low, int64_t high) const;
    int64_t rangeMax(int64_t low, int64_t high) const;
    double rangeAvg(int64_t low, int64_t high) const;
    int64_t rangeCount(int64_t low, int64_t high) const;
    
    int size() const { return size_; }
    size_t memoryUsageBytes() const;

    // Prevent copy
    BPlusTree(const BPlusTree&) = delete;
    BPlusTree& operator=(const BPlusTree&) = delete;
};

} // namespace querylens

#endif // QUERYLENS_BPLUS_TREE_HPP
