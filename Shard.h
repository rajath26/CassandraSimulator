/*
 * Shard.h
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#ifndef SHARD_H_
#define SHARD_H_

#include "Common.h"

class Shard {
private:
	size_t NodeId;
	long logicalClock;
	long val;
	bool isRemoteShard;
public:
	Shard(size_t NodeId_, long logicalClock_, long val_, bool isRemoteShard_);
	virtual ~Shard();
	void setNodeId(size_t NodeId_);
	size_t getNodeId();
	void setLogicalClock(long logicalClock_);
	long getLogicalClock();
	void setVal(long val_);
	long getVal();
	void setIsRemoteShard(bool flag);
	bool getIsRemoteShard();
};

#endif /* SHARD_H_ */
