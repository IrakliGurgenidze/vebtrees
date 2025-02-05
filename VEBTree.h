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
#include <vector>
#include <array>


// 256-bit chunk class definition.
class uint256 {
public:
    std::array<uint64_t, 4> data; // 256-bit chunk represented as 4 uint64_t

    uint256() { data.fill(0); }

    uint64_t& part(int index) {
        return data[index];
    }

    // Function to set the bit at position x in a uint256.
    void set_bit(uint64_t x) {
        int index = x / 64;  // Determine which uint64_t part of the uint256 we need
        int bit_pos = x % 64; // Determine the bit position within that part
        part(index) |= (1ULL << bit_pos); // Set the bit
    }

    // Function to check if the bit at position x in a uint256 is set
    bool is_bit_set(uint64_t x) {
        int index = x / 64;
        int bit_pos = x % 64;
        return (part(index) & (1ULL << bit_pos)) != 0;
    }

    uint64_t next_set_bit(uint64_t x) {
        int part_index = (x + 1) / 64;
        int bit_pos = (x + 1) % 64;     

        for (int i = part_index; i < 4; ++i) {
            uint64_t part_bits = part(i);

            if (i == part_index) {
                uint64_t mask = ~((1ULL << bit_pos) - 1);
                part_bits &= mask;
            }

            if (part_bits != 0) {
                int offset = __builtin_ctzll(part_bits);
                return i * 64 + offset;
            }
        }

        return UINT64_MAX;
    }
};

static const uint8_t MIN_UNIVERSE_SIZE_BITS = 8; // Real universe size == 2^8 = 256

class VEBTree {
public:
    // Constructor to initialize vEB with a given universe size.
    explicit VEBTree(const uint8_t universe_size_bits); // Intended use case if universe size = 2^32 so unierse_size_bits = 32

    // Destructor for our vEB.
    ~VEBTree();

    // Insert key into the tree.
    void insert(uint64_t x);

    // Return whether the key is present in the tree.
    bool query(uint64_t x);

    // Return the successor of key in the tree.
    uint64_t successor(uint64_t x);

private:
    // Size of the universe for this tree.
    uint8_t universe_size_bits;
    uint8_t cluster_size_bits;

    // Bounding elements of a given subtree.
    uint64_t min, max;

    // Bitset to store values with small universe size.
    uint256 bitset;

    // Summary tree for clusters.
    VEBTree* summary;

    // Array of clusters.
    std::vector<VEBTree*> clusters;

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
