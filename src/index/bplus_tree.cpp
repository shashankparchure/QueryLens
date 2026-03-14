#include "../../include/index/bplus_tree.hpp"
#include <algorithm>
#include <limits>
#include <queue>

namespace querylens {

BPlusTree::BPlusTree() : root_(nullptr), size_(0) {}

BPlusTree::~BPlusTree() {
    destroyTree(root_);
}

void BPlusTree::destroyTree(Node* node) {
    if (!node) return;
    if (!node->is_leaf) {
        for (auto child : node->children) {
            destroyTree(child);
        }
    }
    delete node;
}

void BPlusTree::build(const std::vector<Record>& records) {
    // Destroy existing tree
    destroyTree(root_);
    root_ = nullptr;
    size_ = 0;
    
    // Sort records by key and insert
    std::vector<Record> sorted_records = records;
    std::sort(sorted_records.begin(), sorted_records.end(),
              [](const Record& a, const Record& b) { return a.key < b.key; });
    
    for (const auto& rec : sorted_records) {
        insert(rec.key, rec.value);
    }
}

void BPlusTree::insert(int64_t key, int64_t value) {
    Record rec{key, value};
    
    if (!root_) {
        root_ = new Node(true);
        root_->records.push_back(rec);
        root_->keys.push_back(key);
        size_++;
        return;
    }
    
    // Find the leaf node where this key should go
    Node* leaf = findLeaf(key);
    
    // Insert into leaf in sorted order
    auto it = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
    int pos = it - leaf->keys.begin();
    leaf->keys.insert(it, key);
    leaf->records.insert(leaf->records.begin() + pos, rec);
    size_++;
    
    // Split if necessary
    if ((int)leaf->keys.size() >= ORDER) {
        splitLeaf(leaf);
    }
}

BPlusTree::Node* BPlusTree::findLeaf(int64_t key) const {
    Node* cursor = root_;
    while (cursor && !cursor->is_leaf) {
        int i = 0;
        while (i < (int)cursor->keys.size() && key >= cursor->keys[i]) {
            i++;
        }
        cursor = cursor->children[i];
    }
    return cursor;
}

void BPlusTree::splitLeaf(Node* leaf) {
    Node* new_leaf = new Node(true);
    int mid = leaf->keys.size() / 2;
    
    new_leaf->keys.assign(leaf->keys.begin() + mid, leaf->keys.end());
    new_leaf->records.assign(leaf->records.begin() + mid, leaf->records.end());
    leaf->keys.resize(mid);
    leaf->records.resize(mid);
    
    // Link leaves
    new_leaf->next = leaf->next;
    leaf->next = new_leaf;
    
    int64_t split_key = new_leaf->keys[0];
    
    if (leaf == root_) {
        Node* new_root = new Node(false);
        new_root->keys.push_back(split_key);
        new_root->children.push_back(leaf);
        new_root->children.push_back(new_leaf);
        root_ = new_root;
    } else {
        insertInternal(split_key, root_, new_leaf);
    }
}

void BPlusTree::insertInternal(int64_t key, Node* cursor, Node* child) {
    if (cursor->is_leaf) return;
    
    // Find the child that leads to the leaf's parent
    for (int i = 0; i < (int)cursor->children.size(); i++) {
        if (cursor->children[i]->is_leaf) {
            // Check if this is the right parent
            if (i < (int)cursor->keys.size() && key <= cursor->keys[i]) {
                // Found the spot
            } else if (i == (int)cursor->keys.size()) {
                // Last child
            } else {
                continue;
            }
            
            auto it = std::lower_bound(cursor->keys.begin(), cursor->keys.end(), key);
            int pos = it - cursor->keys.begin();
            cursor->keys.insert(it, key);
            cursor->children.insert(cursor->children.begin() + pos + 1, child);
            
            if ((int)cursor->keys.size() >= ORDER) {
                splitInternal(cursor);
            }
            return;
        }
    }
    
    // Recurse to find the correct internal node
    int i = 0;
    while (i < (int)cursor->keys.size() && key >= cursor->keys[i]) i++;
    
    // Check if child belongs under this subtree
    Node* sub = cursor->children[i];
    if (!sub->is_leaf) {
        insertInternal(key, sub, child);
        
        if ((int)sub->keys.size() >= ORDER) {
            splitInternal(sub);
        }
    } else {
        auto it = std::lower_bound(cursor->keys.begin(), cursor->keys.end(), key);
        int pos = it - cursor->keys.begin();
        cursor->keys.insert(it, key);
        cursor->children.insert(cursor->children.begin() + pos + 1, child);
        
        if ((int)cursor->keys.size() >= ORDER) {
            splitInternal(cursor);
        }
    }
}

void BPlusTree::splitInternal(Node* node) {
    if (node != root_) return; // Simplified: only handle root splits for internal nodes
    
    Node* new_node = new Node(false);
    int mid = node->keys.size() / 2;
    int64_t split_key = node->keys[mid];
    
    new_node->keys.assign(node->keys.begin() + mid + 1, node->keys.end());
    new_node->children.assign(node->children.begin() + mid + 1, node->children.end());
    
    node->keys.resize(mid);
    node->children.resize(mid + 1);
    
    if (node == root_) {
        Node* new_root = new Node(false);
        new_root->keys.push_back(split_key);
        new_root->children.push_back(node);
        new_root->children.push_back(new_node);
        root_ = new_root;
    }
}

std::vector<int64_t> BPlusTree::rangeSearch(int64_t low, int64_t high) const {
    std::vector<int64_t> result;
    if (!root_) return result;
    
    Node* leaf = findLeaf(low);
    
    while (leaf) {
        for (int i = 0; i < (int)leaf->records.size(); i++) {
            if (leaf->records[i].key > high) return result;
            if (leaf->records[i].key >= low) {
                result.push_back(leaf->records[i].value);
            }
        }
        leaf = leaf->next;
    }
    
    return result;
}

int64_t BPlusTree::rangeSum(int64_t low, int64_t high) const {
    auto values = rangeSearch(low, high);
    int64_t sum = 0;
    for (auto v : values) sum += v;
    return sum;
}

int64_t BPlusTree::rangeMin(int64_t low, int64_t high) const {
    auto values = rangeSearch(low, high);
    if (values.empty()) return 0;
    return *std::min_element(values.begin(), values.end());
}

int64_t BPlusTree::rangeMax(int64_t low, int64_t high) const {
    auto values = rangeSearch(low, high);
    if (values.empty()) return 0;
    return *std::max_element(values.begin(), values.end());
}

double BPlusTree::rangeAvg(int64_t low, int64_t high) const {
    auto values = rangeSearch(low, high);
    if (values.empty()) return 0.0;
    int64_t sum = 0;
    for (auto v : values) sum += v;
    return static_cast<double>(sum) / values.size();
}

int64_t BPlusTree::rangeCount(int64_t low, int64_t high) const {
    return rangeSearch(low, high).size();
}

size_t BPlusTree::memoryUsageBytes() const {
    if (!root_) return 0;
    size_t total = 0;
    std::queue<Node*> q;
    q.push(root_);
    while (!q.empty()) {
        Node* node = q.front();
        q.pop();
        total += sizeof(Node) + node->keys.capacity() * sizeof(int64_t);
        if (node->is_leaf) {
            total += node->records.capacity() * sizeof(Record);
        } else {
            total += node->children.capacity() * sizeof(Node*);
            for (auto child : node->children) {
                if (child) q.push(child);
            }
        }
    }
    return total;
}

} // namespace querylens
