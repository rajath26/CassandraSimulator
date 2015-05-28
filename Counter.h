/*
 * Counter.h
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#ifndef COUNTER_H_
#define COUNTER_H_

#include "Common.h"
#include "Shard.h"

class Counter {
private:
	std::vector<Shard> shards;
	BFtype bloomFilterType;
public:
	Counter();
	Counter(BFtype type);
	virtual ~Counter();
	void setBFtype(BFtype flag);
	BFtype getBFtype();
	bool addLocalShard(size_t nodeId, int increment);
	bool addRemoteShard(size_t nodeId, long logical_ts, long value);
	long returnSumOfLogicalTsOfAllLocalShards();
	long returnSumOfAllValuesOfAllLocalShards();
	void displayShards();
	std::pair<bool, long> readCounterValue();
	long returnValueOfLatestRemoteShard();
};

#endif /* COUNTER_H_ */
