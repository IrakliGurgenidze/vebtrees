// Author: Prashant Pandey <prashant.pandey@utah.edu>
// For use in CS6968 & CS5968

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

void safe_rand_bytes(unsigned char* v, uint32_t n) {
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
		if (successor != -1 && successor <= out_numbers[i]) {
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

	return 0;
}
