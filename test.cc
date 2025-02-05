/*
 * Irakli Gurgenidze
 * CS 7280
 * 2/5/25
 *
 * Combined test case for VEB Tree and BST.
 */

#include <iostream>
#include <set>
#include <chrono>
#include <string>
#include <openssl/rand.h>

#include "VEBTree.h"

using namespace std::chrono;

double elapsed(high_resolution_clock::time_point t1, high_resolution_clock::time_point t2) {
	return (duration_cast<duration<double>>(t2 - t1)).count();
}

void safe_rand_bytes(unsigned char *v, uint32_t n) {
	while (n > 0) {
		size_t round_size = n >= INT_MAX ? INT_MAX - 1 : n;
		RAND_bytes(v, round_size);
		v += round_size;
		n -= round_size;
	}
}

int main(int argc, char** argv) {
	if (argc < 2) {
		std::cerr << "Specify the number of items for the test.\n";
		exit(1);
	}

	uint32_t N = atoi(argv[1]); 	// number of items

	std::cout << "\nOperation time for " + std::to_string(N / 1000000) + "M items: " << std::endl;

	// Generate N numbers to insert
	uint32_t* in_numbers = (uint32_t*)malloc(N * sizeof(uint32_t));
	if (!in_numbers) {
		std::cerr << "Malloc in_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char*)in_numbers, sizeof(*in_numbers) * N);

	// Generate N numbers different from in_numbers for succ queries
	uint32_t* out_numbers = (uint32_t*)malloc(N * sizeof(uint32_t));
	if (!out_numbers) {
		std::cerr << "Malloc out_numbers failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char*)out_numbers, sizeof(*out_numbers) * N);
	high_resolution_clock::time_point t1, t2;

	/*
	#pragma region BST

	// Create a bst using std::set
	std::set<uint32_t> bst;

	//Insert N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bst.insert(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "[BST] Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Query N items from in_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.find(in_numbers[i]);
		if (ret == bst.end()) {
			std::cerr << "Find in BST failed. Item: " + std::to_string(in_numbers[i]) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "[BST] Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// N Successor queries from out_numbers
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		auto ret = bst.lower_bound(out_numbers[i]);
		if (ret != bst.end() && *ret < out_numbers[i]) {
			std::cerr << "successor query in BST failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(*ret) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "[BST] Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	#pragma endregion

	#pragma region VEBTree

	// Initialize the Van Emde Boas Tree
	VEBTree veb_tree(32);

	// Insert N items into VEBTree
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		veb_tree.insert(in_numbers[i]);
	}
	t2 = high_resolution_clock::now();
	std::cout << "[VEBTree] Time to insert " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Query N items from VEBTree
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		bool found = veb_tree.query(in_numbers[i]);
		if (!found) {
			std::cerr << "Find in VEBTree failed. Item: " + std::to_string(in_numbers[i]) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "[VEBTree] Time to query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Successor queries for VEBTree (test for correctness later to save BST query time)
	t1 = high_resolution_clock::now();
	for (uint32_t i = 0; i < N; ++i) {
		uint32_t successor = veb_tree.successor(out_numbers[i]);
		if (successor != -1 && successor < out_numbers[i]) {
			std::cerr << "[VEBTree] Successor query in VEBTree failed. Item: " + std::to_string(out_numbers[i]) + " Successor: " + std::to_string(successor) + "\n";
			exit(0);
		}
	}
	t2 = high_resolution_clock::now();
	std::cout << "[VEBTree] Time to successor query " + std::to_string(N) + " items: " + std::to_string(elapsed(t1, t2)) + " secs\n";

	// Free allocated memory
	free(in_numbers);
	free(out_numbers);

	#pragma endregion

	#pragma region SUCESSSOR_TEST

	std::cout << "\nBeginning VEB successor accuracy test... \n" << std::endl;
	VEBTree veb(32);

	// Generate 2^14 random numbers [0 - 2^16).
	const uint32_t num_elements = 1 << 14;
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

	// Generate another 2^14 random numbers for successor queries.
	uint32_t* successor_queries = (uint32_t*)malloc(num_elements * sizeof(uint32_t));
	if (!successor_queries) {
		std::cerr << "Malloc succ_queries failed.\n";
		exit(0);
	}
	safe_rand_bytes((unsigned char*)successor_queries, sizeof(*successor_queries) * num_elements);

	// Test successor, verifying the validity of the query results against known values in the tree.
	for (uint32_t i = 0; i < num_elements; i++) {
		uint32_t veb_successor = veb.successor(successor_queries[i]);
		auto bst_successor = bst_reference.lower_bound(successor_queries[i]);

		if ((bst_successor == bst_reference.end() && veb_successor != -1) ||
			(bst_successor != bst_reference.end() && veb_successor != *bst_successor)) {
			std::cerr << "[VEBTree] Successor mismatch for item " + std::to_string(i) + ": " << successor_queries[i]
				<< ". Expected: " << (bst_successor != bst_reference.end() ? std::to_string(*bst_successor) : "empty")
					<< ", Got: " << veb_successor << "\n";
				exit(0);
		}
	}

	std::cout << "[VEBTree] Successor accuracy test " + std::to_string(i + 1) + "/100 passed for " << num_elements << " queries.\n";

	free(test_numbers);
	free(successor_queries);

	#pragma endregion

	return 0;
}
