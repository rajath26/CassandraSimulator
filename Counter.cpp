/*
 * Counter.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#include "Counter.h"

Counter::Counter() {
	shards.clear();
	this->setFBFenable(false);
	if ( this->getFBFenable() ) {
		std::cout<<std::endl<<"FBF is enabled for this Counter object"<<std::endl;
	}
	else {
		std::cout<<std::endl<<"FBF disabled for this Counter object"<<std::endl;
	}
}

Counter::Counter(bool enableFBF) {
	shards.clear();
	this->setFBFenable(enableFBF);
	if ( this->getFBFenable() ) {
		std::cout<<std::endl<<"FBF is enabled for this Counter object"<<std::endl;
	}
	else {
		std::cout<<std::endl<<"FBF disabled for this Counter object"<<std::endl;
	}
}

Counter::~Counter() {}

void Counter::setFBFenable(bool flag) {
	this->enableFBF = flag;
}

bool Counter::getFBFenable() {
	return this->enableFBF;
}

bool Counter::addLocalShard(size_t nodeId, int increment) {
	Shard shardObj(nodeId, 1, increment, false);
	shards.emplace_back(shardObj);
	displayShards();
	return true;
}

bool Counter::addRemoteShard(size_t nodeId_, long logical_ts_, long value_) {
	Shard shardObj(nodeId_, logical_ts_, value_, true);
	shards.emplace_back(shardObj);
	displayShards();
	return true;
}

long Counter::returnSumOfLogicalTsOfAllLocalShards() {
	long sum = 0;
	for ( auto it = shards.begin(); it != shards.end(); it++ ) {
		if ( !((*it).getIsRemoteShard()) ) {
			sum += (*it).getLogicalClock();
		}
	}
	return sum;
}

long Counter::returnSumOfAllValuesOfAllLocalShards() {
	long sum = 0;
	for ( auto it = shards.begin(); it != shards.end(); it++ ) {
		if ( !((*it).getIsRemoteShard()) ) {
			sum += (*it).getVal();
		}
	}
	return sum;
}

long Counter::returnValueOfLatestRemoteShard() {
	long highest = 0;
	long retValue = 0;
	for ( auto it = shards.begin(); it != shards.end(); it++ ) {
		if ( (*it).getIsRemoteShard() ) {
			if ( (*it).getLogicalClock() > highest ) {
				highest = (*it).getLogicalClock();
				retValue = (*it).getVal();
			}
		}
	}
	return retValue;
}

std::pair<bool, long> Counter::readCounterValue() {
	std::cout<<"\n\nSum of local shards: "<<this->returnSumOfAllValuesOfAllLocalShards()<<std::endl;
	std::cout<<"Largest remote shard: "<<this->returnValueOfLatestRemoteShard()<<std::endl<<std::endl;
	return std::make_pair(true, (this->returnSumOfAllValuesOfAllLocalShards() + this->returnValueOfLatestRemoteShard()));
}

void Counter::displayShards() {
	std::cout<<"\n\n";
	std::cout<<"**********************\n";
	std::cout<<"Shards\n";
	std::cout<<"**********************\n";
	std::string str;
	for ( auto &it: shards) {
		if (it.getIsRemoteShard()) {
			str = std::string(REMOTESTRING);
		}
		else {
			str = std::string(LOCALSTRING);
		}
		std::cout<<it.getNodeId()<<"::"<<it.getLogicalClock()<<"::"<<it.getVal()<<"::"<<str<<"\t";
	}
	std::cout<<"\n\n";
	std::cout<<"**********************\n";
	return;
}


