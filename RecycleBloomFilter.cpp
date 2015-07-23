/*
 * RecycleBloomFilter.cpp
 *
 *  Created on: May 27, 2015
 *      Author: rajatsub
 */

#include "RecycleBloomFilter.h"

/**
 * Overloaded Constructor
 */
RecycleBloomFilter::RecycleBloomFilter(unsigned long long int tableSize,
									   unsigned int numberOfHashes,
									   double appProvidedFPP) {
	parameters.projected_element_count = PROJECTED_ELEMENT_COUNT;
	parameters.false_positive_probability = FALSE_POSITIVE_PROBABILITY;
	parameters.random_seed = RANDOM_SEED;
	if ( !parameters ) {
		cout<<endl<<"  ERROR :: Invalid parameters"<<endl;
	}
	parameters.compute_optimal_parameters(tableSize, numberOfHashes);
	bf = bloom_filter(parameters);
	bf.clear();
	cout<<endl<<"  INFO :: Recyclable Bloom Filter initialized"<<endl;
	maxTolerableFPR=appProvidedFPP;
	stopRecycleBloomFilterThread=false;
	thresholdFraction = UNSAFE_THRESHOLD_FRACTION;

	/*
	 * Start the recycle Bloom Filter thread
	 */
	startThreads();
}

/**
 * Destructor
 */
RecycleBloomFilter::~RecycleBloomFilter() {
	stopRecycleBloomFilterThread=true;
	recycleBloomFilterThread.join();
}

/**
 * FUNCTION NAME: insert
 *
 * DESCRIPTION: Insert element into the Recyclable Bloom Filter
 */
void RecycleBloomFilter::insert(unsigned long long int element) {
	lock_guard<mutex> guard(recycleBFMutex);
	bf.insert(element);
}

/**
 * FUNCTION NAME: membershipCheck
 *
 * DESCRIPTION: Membership check function of Recyclable Bloom Filter
 */
bool RecycleBloomFilter::membershipCheck(unsigned long long int element) {
	return bf.contains(element);
}

/**
 * FUNCTION NAME: startThreads
 *
 * DESCRIPTION: This function starts the Bloom Filter recycle thread
 */
void RecycleBloomFilter::startThreads() {
	recycleBloomFilterThread = thread(&RecycleBloomFilter::recycleFunc, this);
}

/**
 * FUNCTION NAME: stopThreads
 *
 * DESCRIPTION: This function stops the Bloom Filter recycle thread
 */
void RecycleBloomFilter::stopThreads() {
	stopRecycleBloomFilterThread=true;
}

/**
 * FUNCTION NAME: recycleFunc
 *
 * DESCRIPTION: Thread function of recycleBloomFilterThread
 */
void RecycleBloomFilter::recycleFunc() {
	cout<<endl<<"  INFO :: Starting the recycle Bloom Filter thread"<<endl;
	while (!stopRecycleBloomFilterThread) {
		recycle();
		this_thread::sleep_for(chrono::seconds(RECYCLE_CHECK_FREQUENCY));
	}
}

/**
 * FUNCTION NAME: recycle
 *
 * DESCRITPION: This function checks the false positive probability
 * 			    of the Bloom filter and if it is approaching the
 * 			    application provided FPP, it recycles the Bloom Filter
 */
void RecycleBloomFilter::recycle() {
	double currentFPP=checkFPP();
	//std::cout<<std::endl<<"  INFO :: Checking FPP of recyclable Bloom Filter"<<std::endl;
	if ( currentFPP >  maxTolerableFPR ) {
		recycleThisBloomFilter();
	}
}

/**
 * FUNCTION NAME: checkFPP
 *
 * DESCRIPTION: This function checks the FPP of the Bloom Filter
 */
double RecycleBloomFilter::checkFPP() {
	return bf.effective_fpp();
}

/**
 * FUNCTION NAME: recycleBloomFilter
 *
 * DESCRIPTION: This function recycles the Bloom Filter
 */
void RecycleBloomFilter::recycleThisBloomFilter() {
	lock_guard<mutex> guard(recycleBFMutex);
	std::cout<<std::endl<<"  INFO :: Recycling Bloom Filter"<<std::endl;
	std::cout<<std::endl<<"  INFO :: Current FPP: "<<this->checkFPP()<<std::endl;
	bf.clear();
}

