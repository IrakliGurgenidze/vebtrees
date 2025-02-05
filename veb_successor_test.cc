/*
 * Irakli Gurgenidze
 * CS 7280
 * 2/5/25
 *
 * Successor correctness check for VEB Tree.
 */

#include <iostream>
#include <set>
#include <chrono>
#include <string>
#include <openssl/rand.h>

#include "VEBTree.h"

using namespace std::chrono;

void safe_rand_bytes(unsigned char* v, uint32_t n) {
	while (n > 0) {
		size_t round_size = n >= INT_MAX ? INT_MAX - 1 : n;
		RAND_bytes(v, round_size);
		v += round_size;
		n -= round_size;
	}
}

int main(int argc, char** argv) {

	#pragma region SUCESSSOR_TEST

	std::cout << "\nBeginning VEB successor accuracy test for 2^15 elements..." << std::endl;
	
	VEBTree veb(32);

	// Generate 2^15 random numbers [0 - 2^16).
	const uint32_t num_elements = 1 << 15;
	uint32_t* test_numbers = (uint32_t*)malloc(num_elements * sizeof(uint32_t));
	if (!test_numbers) {
		std::cerr << "Malloc test_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char*)test_numbers, sizeof(*test_numbers) * num_elements);

	// Insert them into veb and bst.
	std::set<uint32_t> bst_reference;
	for (uint32_t i = 0; i < num_elements; i++) {
		veb.insert(test_numbers[i]);
		bst_reference.insert(test_numbers[i]);
	}

	// Generate another 2^15 random numbers for successor queries.
	uint32_t* successor_queries = (uint32_t*)malloc(num_elements * sizeof(uint32_t));
	if (!successor_queries) {
		std::cerr << "Malloc succ_queries failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char*)successor_queries, sizeof(*successor_queries) * num_elements);

	// Test successor, verifying the validity of the query results against known values in the tree.
	for (uint32_t i = 0; i < num_elements; i++) {
		uint32_t veb_successor = veb.successor(successor_queries[i]);
		auto bst_successor = bst_reference.upper_bound(successor_queries[i]);

		if ((bst_successor == bst_reference.end() && veb_successor != -1) ||
			(bst_successor != bst_reference.end() && veb_successor != *bst_successor)) {
			std::cerr << "[VEBTree] Successor mismatch for item " + std::to_string(i) + ": " << successor_queries[i]
				<< ". Expected: " << (bst_successor != bst_reference.end() ? std::to_string(*bst_successor) : "empty")
					<< ", Got: " << veb_successor << "\n";
				exit(0);
		}
	}

	free(test_numbers);
	free(successor_queries);
	
	std::cout << "Successor test completed with no issues." << std::endl;	

	#pragma endregion

	return 0;
}
