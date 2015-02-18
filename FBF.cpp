/*
 * FBF.cpp
 *
 *  Created on: Nov 20, 2014
 *      Author: rajatsub
 */

#include "FBF.h"

/**
 * Overloaded Constructor
 */
FBF::FBF(unsigned long number, unsigned long long int tableSize, unsigned int numOfHashes, unsigned long refreshTime, double appProvidedFPR, bool enableDynamicResizing) {
	//trace.funcEntry("FBF::FBF");
	parameters.projected_element_count = PROJECTED_ELEMENT_COUNT;
	parameters.false_positive_probability = FALSE_POSITIVE_PROBABILITY;
	parameters.random_seed = RANDOM_SEED;
	if ( !parameters ) {
		cout<<"  ERROR :: Invalid parameters"<<endl;
	}
	parameters.compute_optimal_parameters(tableSize, numOfHashes);
	fbf.clear();
	for ( int counter = 0; counter < number; counter++ ) {
		fbf.emplace_back(bloom_filter(parameters));
		fbf[counter].clear();
	}
	cout<<"  INFO :: "<<fbf.size()<<" constituent bloom filters initialized in the FBF"<<endl;

	future = FUTURE;
	present = future + 1;
	pastStart = future + 2;
	numberOfBFs = fbf.size();
	pastEnd = numberOfBFs - 1;
	refreshRate = refreshTime;
	maxTolerableFPR = appProvidedFPR;
	this->enableDynamicResizing = enableDynamicResizing;
	stopRefreshFBFThread = false;
	stopdynamicResizingThread = false;
	thresholdFraction = UNSAFE_THRESHOLD_FRACTION;
	safeThresholdFraction = SAFE_THRESHOLD_FRACTION;
	multiplicativeBloomFilterIncrement = MULTIPLICATIVE_BLOOM_FILTER_INCREMENT;
	//additiveBloomFilterIncrement = ADDITIVE_BLOOM_FILTER_INCREMENT;
	minRefreshRate = MIN_REFRESH_RATE;
	additiveRefreshRateDecrement = ADDITIVE_REFRESH_RATE_DECREMENT;
	minimumNumberOfBFs = MINIMUM_NUMBER_OF_BFS;
	additiveBloomFilterDecrement = ADDITIVE_BLOOM_FILTER_DECREMENT;
	additiveRefreshRateIncrement = ADDITIVE_REFRESH_RATE_INCREMENT;

	cout<<"  INFO :: FBFs initialized"<<endl;

	/*
	 * Start the refresh FBF and dynamic resizing threads
	 */
	startThreads();

	//trace.funcExit("FBF::FBF");
}

/**
 * Destructor
 */
FBF::~FBF() {
	//trace.funcEntry("FBF::~FBF");
	stopRefreshFBFThread = true;
	stopdynamicResizingThread = true;
	refreshFBFThread.join();
	if ( enableDynamicResizing ) {
		dynamicResizingThread.join();
	}
	//trace.funcExit("FBF::~FBF");
}

/**
 * FUNCTION NAME: insert
 *
 * DESCRIPTION: Insert element into the FBF
 */
void FBF::insert(unsigned long long int element) {
	//trace.funcEntry("FBF::insert");
	lock_guard<std::mutex> guard(fbfMutex);
	fbf[present].insert(element);
	fbf[future].insert(element);
	//trace.funcExit("FBF::insert");
}

/**
 * FUNCTION NAME: membershipCheck
 *
 * DESCRIPTION: Membership check function of the FBF
 */
bool FBF::membershipCheck(unsigned long long int element) {
	//trace.funcEntry("FBF::membershipCheck");
	bool found = false;
	unsigned long lastPast = getPastEnd();
	if ( fbf[future].contains(element) && fbf[present].contains(element) ) {
		found = true;
	}
	else if ( fbf[present].contains(element) && fbf[pastStart].contains(element) ) {
		found = true;
	}
	else if ( lastPast > pastStart ) {
		for ( int j = pastStart; j <= (lastPast - 1); j++ ) {
			if ( (fbf[j].contains(element) && fbf[j+1].contains(element)) ) {
				found = true;
				break;
			}
		}
	}
	else if ( fbf[lastPast].contains(element) ) {
		found = true;
	}
	//trace.funcExit("FBF::membershipCheck");
	return found;
}

/**
 * FUNCTION NAME: startThreads
 *
 * DESCRIPTION: This function starts the FBF refresh as well as the dynamic resizing
 * 				threads
 */
void FBF::startThreads() {
	//trace.funcEntry("FBF::startThreads");
	refreshFBFThread = thread(&FBF::refreshFunc, this);
	if ( enableDynamicResizing ) {
		dynamicResizingThread = thread(&FBF::dynamicResizingFunc, this);
	}
	//trace.funcExit("FBF::startThreads");
}

/**
 * FUNCTION NAME: stopThreads
 *
 * DESCRIPTION: This function stops the FBF refresh as well as the dyanmic resizing threads
 */
void FBF::stopThreads() {
	//trace.funcEntry("FBF::stopThreads");
	stopRefreshFBFThread = true;
	stopdynamicResizingThread = true;
	//trace.funcExit("FBF::stopThreads");
}

/**
 * FUNCTION NAME: refreshFunc
 *
 * DESCRIPTION: Thread function of refreshFBFThread
 */
void FBF::refreshFunc() {
	//trace.funcEntry("FBF::refreshFunc");
	cout<<endl<<"  INFO :: Starting the refresh FBF thread"<<endl;
	while (!stopRefreshFBFThread) {
		refresh();
		this_thread::sleep_for(chrono::seconds(getRefreshRate()));
	}
	//trace.funcExit("FBF::refreshFunc");
}

/**
 * FUNCTION NAME: refresh
 *
 * DESCRIPTION: This function refreshes the FBF
 */
void FBF::refresh() {
	//trace.funcEntry("FBF::refresh");
	lock_guard<std::mutex> guard(fbfMutex);
	fbf.insert(fbf.begin(), bloom_filter(parameters));
	fbf.pop_back();
	cout<<"  INFO :: Refreshed FBF"<<endl;
	//trace.funcExit("FBF::refresh");
}

/**
 * FUNCTION NAME: dynamicResizingFunc
 *
 * DESCRIPTION: This function controls the dynamic resizing functionality
 */
void FBF::dynamicResizingFunc() {
	//trace.funcEntry("FBF::dynamicResizingFunc");
	cout<<endl<<"  INFO :: Starting dynamic resizing thread"<<endl;
	while (!stopdynamicResizingThread) {
		checkFalsePositiveRate();
		this_thread::sleep_for(chrono::seconds(DYNAMIC_RESIZING_CHECK_FREQUENCY_SECONDS));
	}
	//trace.funcExit("FBF::dynamicResizingFunc");
}

/**
 * FUNCTION NAME: checkFalsePositiveRate
 *
 * DESCRIPTION: This function checks the current false positive rate of the FBF
 * 				and decides whether to trigger dynamic resizing
 */
void FBF::checkFalsePositiveRate() {
	//trace.funcEntry("FBF::checkFalsePositiveRate");
	double currentFPR = checkEffectiveFPR();
	if ( currentFPR > thresholdFraction * maxTolerableFPR ) {
		scaleUp();
	}
	else if ( currentFPR <= safeThresholdFraction * maxTolerableFPR ) {
		scaleDown();
	}
	//trace.funcExit("FBF::checkFalsePositiveRate");
}

/**
 * FUNCTION NAME: scaleUp
 *
 * DESCRIPTION: This function expands the FBF and decreases the refresh rate
 */
void FBF::scaleUp() {
	//trace.funcEntry("FBF::scaleUp");
	fbfScaleUp();
	numberOfBFs = fbf.size();
	pastEnd = numberOfBFs - 1;
	if ( getRefreshRate() > minRefreshRate ) {
		setRefreshRate(getRefreshRate() - additiveRefreshRateDecrement);
	}
	cout<<endl<<"new rr: "<<getRefreshRate()<<endl;
	cout<<endl<<"scale up"<<endl;
	//trace.funcExit("FBF::scaleUp");
}

/**
 * FUNCTION: fbfScaleUp
 *
 * DESCRIPTION: This function expands the FBF
 */
void FBF::fbfScaleUp() {
	size_t currentFBFsize = fbf.size();
	lock_guard<std::mutex> guard(fbfMutex);
	for ( int counter = currentFBFsize; counter < currentFBFsize * multiplicativeBloomFilterIncrement; counter++ ) {
		fbf.emplace_back(bloom_filter(parameters));
	}
}

/**
 * FUNCTION NAME: scaleDown
 *
 * DESCRIPTION: THis function shrinks the FBF and increases the refresh rate
 */
void FBF::scaleDown() {
	//trace.funcEntry("FBF::scaleDown");
	size_t currentFBFsize = fbf.size();
	if ( currentFBFsize > (minimumNumberOfBFs + additiveBloomFilterDecrement) ) {
		fbfScaleDown();
	}
	numberOfBFs = fbf.size();
	pastEnd = numberOfBFs - 1;
	setRefreshRate(getRefreshRate() + additiveRefreshRateIncrement);
	//trace.funcExit("FBF::scaleDown");
}

/**
 * FUNCTION NAME: fbfScaleDown
 *
 * DESCRIPTION: This function shrinks the FBF
 */

void FBF::fbfScaleDown() {
	lock_guard<std::mutex> guard(fbfMutex);
	for ( int counter = 0; counter < additiveBloomFilterDecrement; counter++ ) {
		fbf.pop_back();
	}
}

/**
 * FUNCTION NAME: getRefreshRate
 *
 * DESCRIPTION: This function returns the refresh rate of the FBF
 */
unsigned long FBF::getRefreshRate() {
	//trace.funcEntry("FBF::getRefreshRate");
	//trace.funcExit("FBF::getRefreshRate", (int)this->refreshRate);
	lock_guard<std::mutex> guard(rrMutex);
	return this->refreshRate;
}

/**
 * FUNCTION NAME: setRefreshRate
 *
 * DESCRIPTION: This function sets the refrehs rate of the FBF
 */
bool FBF::setRefreshRate(unsigned long newRR) {
	//trace.funcEntry("FBF::setRefreshRate");
	lock_guard<std::mutex> guard(rrMutex);
	this->refreshRate = newRR;
	//trace.funcExit("FBF::setRefreshRate");
	return true;
}

/**
 * FUNCTION NAME: getPastEnd
 *
 * DESCRIPTION: THis function returns the index of the Past end bloom filter
 */
unsigned long FBF::getPastEnd() {
	//trace.funcEntry("FBF::getPastEnd");
	numberOfBFs = fbf.size();
	pastEnd = numberOfBFs - 1;
	//trace.funcExit("FBF::getPastEnd", (int)this->pastEnd);
	return this->pastEnd;
}

/**
 * FUNCTION NAME: checkSmartFBF_FPR
 *
 * DESCRIPTION: This function checks the False Positives (FPs) and the
 *              False Positive Rate (FPR) of the FBF using SMART RULES
 */
double FBF::checkSmartFBF_FPR(unsigned long long int number, testType test) {
	//trace.funcEntry("FBF::checkSmartFBF_FPR");
	unsigned long long int smartFP = 0;
	double smartFPR = 0.0;
	long long int i = -1;
	unsigned long long int counter = 0;
	if ( test == falsePositiveNumber ) {
		unsigned long long int tries = 0;
		while ( counter != number ) {
			if ( membershipCheck(i) ) {
				smartFP++;
				counter++;
			}
			i--;
			tries++;
		}
		smartFPR = (double) smartFP/tries;
	}
	else if ( test == numberOfTrials ) {
		while ( counter != number ) {
			if ( membershipCheck(i) ) {
				smartFP++;
			}
			i--;
			counter++;
		}
		smartFPR = (double) smartFP/counter;
		if ( 0 == smartFP ) {
			smartFPR = checkSmartFBF_FPR(FALSE_POSITIVES_CHECK, falsePositiveNumber);
		}
	}
	cout<<"  RESULT :: Smart FPR = "<<smartFPR<<endl;
	//trace.funcExit("FBF::checkSmartFBF_FPR", (int)smartFPR);
	return smartFPR;
}

/**
 * FUNCTION NAME: checkDumbFBF_FPR
 *
 * DESCRIPTION: This function checks the False Positives (FPs) and the
 * 			    False Positive Rate (FPR) of the FBF using NAIVE RULES
 */
double FBF::checkDumbFBF_FPR(unsigned long long int number, testType test) {
	//trace.funcEntry("FBF::checkDumbFBF_FPR");
	unsigned long long int dumbFP = 0;
	double dumbFPR = 0.0;
	long long int i = -1;
	unsigned long long int counter = 0;
	int j = 0;
	unsigned long lastPast = getPastEnd();
	if ( test == falsePositiveNumber ) {
		unsigned long long int tries = 0;
		while ( counter != number ) {
			for ( j = future; j <= lastPast; j++ ) {
				if ( fbf[j].contains(i) ) {
					dumbFP++;
					counter++;
					break;
				}
			}
			i--;
			tries++;
		}
		dumbFPR = (double) dumbFP/tries;
	}
	else if ( test == numberOfTrials ) {
		while ( counter != number ) {
			for ( j = future; j <= lastPast; j++ ) {
				if ( fbf[j].contains(i) ) {
					dumbFP++;
					break;
				}
			}
			i--;
			counter++;
		}
		dumbFPR = (double) dumbFP/counter;
		if ( 0 == dumbFP ) {
			dumbFPR = checkDumbFBF_FPR(FALSE_POSITIVES_CHECK, falsePositiveNumber);
		}
	}
	cout<<" RESULT :: DUMB FPR = " <<dumbFPR <<endl;
	//trace.funcExit("FBF::checkDumbFBF_FPR", (int)dumbFPR);
	return dumbFPR;
}

/**
 * FUNCTION NAME: checkEffectiveFPR
 *
 * DESCRIPTION: This function returns the False Positive Rate based
 * 				on the conditional probability rules
 */
double FBF::checkEffectiveFPR() {
	//trace.funcEntry("FBF::checkEffectiveFPR");
	unsigned int counter = 0;
	unsigned int temp = 0;
	double effectiveFPR = 0.0;
	unsigned long lastPast = getPastEnd();

	// Debug
	/*
	cout<<endl<<"  INFO :: Individual FPR here: "<<endl;
	cout<<"  INFO :: Future BF FPR: "<<fbf[future].effective_fpp()<<endl;
	for ( int i = present; i <= lastPast; i++ ) {
		cout<<"  INFO :: " <<i<<"BF FPR: "<<fbf[i].effective_modified_fpp()<<endl;
	}
	*/

	//effectiveFPR = ( fbf[future].effective_fpp() * fbf[present].effective_modified_fpp());
	effectiveFPR = ( 1 - fbf[future].effective_fpp());
	for ( counter = present; counter <= (lastPast - 1); counter++ ) {
		temp = counter + 1;
		//effectiveFPR += (fbf[counter].effective_modified_fpp() * fbf[temp].effective_modified_fpp());
		//effectiveFPR *= (1 - (fbf[counter].effective_modified_fpp() * fbf[temp].effective_modified_fpp()));
		effectiveFPR *= (1 - (fbf[counter].effective_fpp() * fbf[temp].effective_fpp()));
	}

	//effectiveFPR += (fbf[lastPast].effective_modified_fpp());
	//effectiveFPR *= (1 - (fbf[lastPast].effective_modified_fpp()));
	effectiveFPR *= (1 - (fbf[lastPast].effective_fpp()));

	effectiveFPR = 1 - effectiveFPR;

	cout<<" RESULT :: Effective FPR: " <<effectiveFPR <<endl;
	//trace.funcExit("FBF::checkEffectiveFPR", (int)effectiveFPR);
	return effectiveFPR;
}

/**
 * FUNCTION NAME: fbfAnalysis
 *
 * DESCRIPTION: This function performs the FBF analysis
 */
double FBF::fbfAnalysis() {
	//trace.funcEntry("FBF::fbfAnalysis");
	vector<int> n;
	for ( int i = 0 ; i < fbf.size(); i++ ) {
		n.emplace_back(populateNi(i));
		cout<<"Elements in : "<<i<<" filter: "<<n[i]<<endl;
	}

	double fbfFPR = 0.0;

	fbfFPR = ( 1 - fbf[future].effective_n_given_fpp(n[future], fbf[future].getSaltSize()));
	fbfFPR *= 1 - fbf[present].effective_n_given_fpp(n[present], (int)(fbf[present].getSaltSize()));
	fbfFPR *= (1 - fbf[present].effective_n_given_fpp(n[present], (int)(fbf[present].getSaltSize())))*(1 - fbf[present].effective_n_given_fpp(n[present], (int)(fbf[present].getSaltSize() - Hi(future))) * fbf[pastStart].effective_n_given_fpp(n[pastStart], (int)Hi(future)));
	fbfFPR *= 1 - fbf[pastStart].effective_n_given_fpp(n[pastStart], (int)(fbf[pastStart].getSaltSize() - Hi(present)));
	double finalFP = 1 - fbfFPR;
	cout<<"  INFO :: fbf Analysis FPR: " <<finalFP<<endl;
	//trace.funcExit("FBF::fbfAnalysis", (int)finalFP);
	return finalFP;
}

/**
 * FUNCTION NAME: populateNi
 *
 * DESCRIPTION: This function populates the number of elements inserted into each bloom filter
 */
int FBF::populateNi(int indexOfBloomFilter) {
	//trace.funcEntry("FBF::populateNi");
	if ( -1 == indexOfBloomFilter ) {
		//trace.funcExit("FBF::populateNi", 0);
		return 0;
	}
	else {
		//trace.funcExit("FBF::populateNi", SUCCESS);
		return fbf[indexOfBloomFilter].getInsertedElementCount() - populateNi(indexOfBloomFilter - 1);
	}
}

/**
 * FUNCTION NAME: Hi
 *
 * DESCRIPTION: This function is a helper function
 */
double FBF::Hi(int bloomFilterIndex) {
	//trace.funcEntry("FBF::Hi");
	if ( -1 == bloomFilterIndex ) {
		//trace.funcExit("FBF::Hi");
		return 0;
	}
	else {
		//trace.funcExit("FBF::Hi");
		//return expectedValue(populateNi(bloomFilterIndex), fbf[present].getSaltSize() - Hi(bloomFilterIndex-1));
		// 12/02/14
		return expectedValue(populateNi(bloomFilterIndex), fbf[present].getSaltSize() - 1);
	}
}

/**
 * FUNCTION NAME: expectedValue
 *
 * DESCRIPTION: Helper function
 */
double FBF::expectedValue(int p, int l) {
	//trace.funcEntry("FBF::expectedValue");
	double baseProb = std::exp(-1.0 * fbf[present].getSaltSize() * p /fbf[present].size());
	double sum_e = 0.0;
	for ( int i = 0; i <= l; i++ ) {
		sum_e += i * std::pow((1 - baseProb), i) * std::pow((baseProb), (l-i));
	}
	//trace.funcExit("FBF::expectedValue", (int)sum_e);
	return sum_e;
}
