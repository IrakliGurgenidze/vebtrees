/*
 * Irakli Gurgenidze
 * CS 7280
 * 2/5/25
 *
 * Header for a vEB tree implementation.
 */

#ifndef VEB_H
#define VEB_H

#include <cstdint>
#include <bitset>
#include <unordered_map>

static const uint8_t MIN_UNIVERSE_SIZE_BITS = 4; // Real universe size == 2^4 = 16

class VEBTree {
public:
    // Constructor to initialize vEB with a given universe size.
    explicit VEBTree(const uint8_t universe_size_bits); // Intended use case if universe size = 2^32 so unierse_size_bits = 32

    // Destructor for our vEB.
    ~VEBTree();

    // Insert key into the tree.
    void insert(uint64_t x);

    // Return whether the key is present in the tree.
    bool query(uint64_t x) const;

    // Return the successor of key in the tree.
    uint64_t successor(uint64_t x) const;

private:
    // Size of the universe for this tree.
    uint8_t universe_size_bits;
    uint8_t cluster_size_bits;

    // Bounding elements of a given subtree.
    uint64_t min, max;

    // Bitset to store values with small universe size.
    uint16_t bitset;

    // Summary tree for clusters.
    VEBTree* summary;

    // Array of clusters.
    std::unordered_map<uint32_t, VEBTree*> clusters;

    // Compute cluster_size as 2^cluster_size_bits.
    inline uint64_t cluster_size() const {
        return static_cast<uint64_t>(1) << cluster_size_bits; // 2^cluster_size_bits
    }

    // Compute x_H.
    inline uint64_t high(uint64_t x) const {
        return x >> cluster_size_bits;
    };

    // Compute x_L.
    inline uint64_t low(uint64_t x) const {
        return x & (cluster_size() - 1);
    };

    // Compute index of an element within the tree.
    inline uint64_t index(const uint64_t high, const uint64_t low) const {
        return (high << cluster_size_bits) | low;
    };
};

#endif //VEB_H
