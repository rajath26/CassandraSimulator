/*
 * FBF.h
 *
 *  Created on: Nov 20, 2014
 *      Author: rajatsub
 */

#ifndef FBF_H_
#define FBF_H_

/*
 * Header files
 */
#include "Common.h"
//#include<thread>
//#include<mutex>
//#include<chrono>

/*
 * Bloom Filter Library
 */
#include "bloom_filter.hpp"

/*
 * Trace class
 */
//#include "Trace.h"

/*
 * Macros
 */
#define PROJECTED_ELEMENT_COUNT 10000
#define FALSE_POSITIVE_PROBABILITY 0.0001
#define RANDOM_SEED 0xA5A5A5A5
#define FUTURE 0
#define DYNAMIC_RESIZING_CHECK_FREQUENCY_SECONDS 1
#define UNSAFE_THRESHOLD_FRACTION 0.6
#define SAFE_THRESHOLD_FRACTION 0.001
#define MULTIPLICATIVE_BLOOM_FILTER_INCREMENT 2
#define ADDITIVE_BLOOM_FILTER_INCREMENT 3
#define ADDITIVE_BLOOM_FILTER_DECREMENT 1
#define MIN_REFRESH_RATE 1
#define ADDITIVE_REFRESH_RATE_DECREMENT 1
#define ADDITIVE_REFRESH_RATE_INCREMENT 1
#define MINIMUM_NUMBER_OF_BFS 3
#define FALSE_POSITIVES_CHECK 2

/*
 * Global variables
 */
//extern Trace trace;
enum testType {falsePositiveNumber, numberOfTrials};

/**
 * CLASS NAME: FBF
 *
 * DESCRIPTION: This class implements a Forgetful Bloom Filter
 */
class FBF {
private:
	// Bloom Filter parameters
	bloom_parameters parameters;
	// Constituent bloom filters of the FBF
	// Future, Present, A set of Past Bloom Filter(s)
	vector<bloom_filter> fbf;
	// Number of bloom filters
	unsigned long numberOfBFs;
	// Future bloom filter
	unsigned long future;
	// Present bloom filter
	unsigned long present;
	// Past start bloom filter
	unsigned long pastStart;
	// Past end bloom filter
	unsigned long pastEnd;
	// Refresh rate
	unsigned long refreshRate;
	// Maximum tolerable false positive rate
	double maxTolerableFPR;
	// Thread to refresh FBF
	thread refreshFBFThread;
	// Thread to do dynamic resizing
	thread dynamicResizingThread;
	// Switch to turn off dynamic resizing
	bool enableDynamicResizing;
	// flag to stop refreshFBF thread
	bool stopRefreshFBFThread;
	// flag to stop dynamic resizing thread
	bool stopdynamicResizingThread;
	// mutex to lock FBF
	// FBF is accessed by refresh thread, insert thread and dynamic resizing thread\
	// refresh thread and insert thread operate on future BF and need to be mutually exclusive
	// refresh thread and dynamic resizing thread operate on pastEnd BF and need to be mutually exclusive
	mutex fbfMutex;
	// mutex to lock refreshRate
	// refreshRate is accessed by refresh thread, dynamic resizing thread and also drive thread
	mutex rrMutex;
	// unsafe threshold fraction of maxTolerableFPP
	double thresholdFraction;
	// safe threshold fraction of maxTolerableFPP
	double safeThresholdFraction;
	// multiplicative bloom filter increment
	unsigned int multiplicativeBloomFilterIncrement;
	// additive bloom filter increment
	unsigned int additiveBloomFilterIncrement;
	// additive bloom filter decrement
	unsigned int additiveBloomFilterDecrement;
	// minimum refreshRate
	unsigned int minRefreshRate;
	// additive refreshRate decrement
	unsigned int additiveRefreshRateDecrement;
	// additive refreshRate increment
	unsigned int additiveRefreshRateIncrement;
	// Minimum number of constituent bloom filters
	unsigned int minimumNumberOfBFs;

public:
	FBF(unsigned long numberOfBFs,
		unsigned long long int tableSize,
		unsigned int numberOfHashes,
		unsigned long refreshTime,
		double appProvidedFPP,
		bool enableDynamicResizing);
	virtual ~FBF();
	void insert(unsigned long long int element);
	bool membershipCheck(unsigned long long int element);
	void startThreads();
	void stopThreads();
	void refreshFunc();
	void refresh();
	void dynamicResizingFunc();
	void checkFalsePositiveRate();
	void scaleUp();
	void fbfScaleUp();
	void scaleDown();
	void fbfScaleDown();
	unsigned long getRefreshRate();
	bool setRefreshRate(unsigned long newRefreshRate);
	unsigned long getPastEnd();
	double checkSmartFBF_FPR(unsigned long long int numberOfInvalids, testType test);
	double checkDumbFBF_FPR(unsigned long long int numberOfInvalids, testType test);
	double checkEffectiveFPR();
	double fbfAnalysis();
	int populateNi(int indexOfBloomFilter);
	int ni(int i);
	double Hi(int i);
	double expectedValue(int n, int offset);
};

#endif /* FBF_H_ */
