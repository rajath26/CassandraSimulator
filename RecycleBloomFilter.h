/*
 * RecycleBloomFilter.h
 *
 *  Created on: May 27, 2015
 *      Author: rajatsub
 */

#ifndef RECYCLEBLOOMFILTER_H_
#define RECYCLEBLOOMFILTER_H_

/*
 * Header files
 */
#include "Common.h"
#include<thread>
#include<mutex>
#include<chrono>

/*
 * Bloom Filter Library
 */
#include "bloom_filter.hpp"

/*
 * Macros
 */
#define PROJECTED_ELEMENT_COUNT 10000
#define FALSE_POSITIVE_PROBABILITY 0.0001
#define RANDOM_SEED 0xA5A5A5A5
#define UNSAFE_THRESHOLD_FRACTION 0.9
#define RECYCLE_CHECK_FREQUENCY 1

/**
 * CLASS NAME: RecycleBloomFilter
 *
 * DESCRIPTION: This class implements a Recycle Bloom Filter
 */
class RecycleBloomFilter {
private:
	// Bloom Filter parameters
	bloom_parameters parameters;
	// Bloom Filter
	bloom_filter bf;
	// Maximum tolerable false positive rate
	double maxTolerableFPR;
	// Thread to recycle Bloom Filter as the FPP approaches the target
	thread recycleBloomFilterThread;
	// Flag to turn off recycle Bloom Filter thread
	bool stopRecycleBloomFilterThread;
	// mutex for refresh Bloom Filter thread
	mutex recycleBFMutex;
	// unsafe threshold fraction of maxTolerableFPP
	double thresholdFraction;
public:
	RecycleBloomFilter(unsigned long long int tableSize,
			           unsigned int numberOfHashes,
					   double appProvidedFPP);
	virtual ~RecycleBloomFilter();
	void insert(unsigned long long int element);
	bool membershipCheck(unsigned long long int element);
	void startThreads();
	void stopThreads();
	void recycleFunc();
	void recycle();
	double checkFPP();
	void recycleThisBloomFilter();
};

#endif /* RECYCLEBLOOMFILTER_H_ */
