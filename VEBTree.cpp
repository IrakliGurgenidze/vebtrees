/*
 * Irakli Gurgenidze
 * CS 7280
 * 2/5/25
 *
 * Implementation for a vEB tree implementation.
 */

#include "VEBTree.h"
#include <cmath>
#include <climits>
#include <iostream>
#include <chrono>
#include <string>

static const uint64_t EMPTY = UINT64_MAX;

VEBTree::VEBTree(const uint8_t universe_size_bits) {
    this->universe_size_bits = universe_size_bits;
    this->cluster_size_bits = universe_size_bits / 2;

    // min and max are set to invalid values initially.
    this->min = EMPTY;
    this->max = EMPTY;

    // Base case ~ universe size is at lower bound.
    if (universe_size_bits == MIN_UNIVERSE_SIZE_BITS) {
        bitset = 0;
        return;
    }

    this->summary = nullptr;
    clusters.resize(cluster_size(), nullptr);
}

VEBTree::~VEBTree() {
    delete summary;
    for (VEBTree* cluster : clusters) {
        delete cluster;
    }
}

/* Public methods. */
void VEBTree::insert(uint64_t x) {
    // Base case ~ minimum universe size.
    if (universe_size_bits <= MIN_UNIVERSE_SIZE_BITS) {
        if (min == EMPTY) {
            min = max = x;
        }
        else {
            if (x < min) min = x;
            if (x > max) max = x;
        }

        bitset |= (1 << x);
        return;
    }

    // Base case ~ cluster is EMPTY.
    if (min == EMPTY) {
        min = max = x;
        return;
    }

    if (x < min) std::swap(x, min);
    if (x > max) max = x;

    const uint64_t xh = high(x);
    const uint64_t xl = low(x);

    if (!summary) {
        this->summary = new VEBTree(cluster_size_bits);
    }

    VEBTree* cluster_xh = clusters[xh];
    if (!cluster_xh) {
        cluster_xh = new VEBTree(cluster_size_bits);
        clusters[xh] = cluster_xh;
        summary->insert(xh);
    }

    cluster_xh->insert(xl);
}

bool VEBTree::query(uint64_t x) const {
    // Base case ~ we have recursed to min cluster size.
    if (universe_size_bits <= MIN_UNIVERSE_SIZE_BITS) {
        return (bitset & (1 << x)) != 0;
    }

    // Base case ~ x == min or max, and therefore present in tree.
    if (x == min || x == max) return true;

    const uint64_t xh = high(x);

    VEBTree* cluster_xh = clusters[xh];
    if (!cluster_xh) {
        return false;
    }

    const uint64_t xl = low(x);
    return cluster_xh->query(xl);
}

uint64_t VEBTree::successor(uint64_t x) const {
    // Base case ~ universe size is at min bound.
    if (universe_size_bits <= MIN_UNIVERSE_SIZE_BITS) {
        if (x >= max) return EMPTY;
        if (x < min) return min;

        for (uint64_t i = x + 1; (1 << MIN_UNIVERSE_SIZE_BITS); i++) {
            if (bitset & (1 << i)) return i; // Find the next set bit.
        }

        return EMPTY;
    }

    // Base case ~ if x > max, return invalid.
    if (x >= max) return EMPTY;

    // Base case ~ if x outside bounds, successor is min.
    if (x < min) return min;

    uint64_t xh = high(x);
    uint64_t xl = low(x);

    VEBTree* cluster_xh = clusters[xh];
    if (cluster_xh) {
        uint64_t answer = cluster_xh->successor(xl);
        if (answer != EMPTY) {
            return index(xh, answer);
        }
    }

    uint64_t answerCluster = summary ? summary->successor(xh) : EMPTY;
    if (answerCluster == EMPTY) {
        return EMPTY;
    }

    if (!clusters[answerCluster]) return EMPTY;
    return index(answerCluster, clusters[answerCluster]->min);
}
