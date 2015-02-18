/*
 * Shard.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#include "Shard.h"

Shard::Shard(size_t NodeId_, long logicalClock_, long val_, bool isRemoteShard_) {
	this->setNodeId(NodeId_);
	this->setLogicalClock(logicalClock_);
	this->setVal(val_);
	this->setIsRemoteShard(isRemoteShard_);
}

Shard::~Shard() {}

void Shard::setNodeId(size_t NodeId_) {
	this->NodeId = NodeId_;
}

size_t Shard::getNodeId() {
	return this->NodeId;
}

void Shard::setLogicalClock(long logicalClock_) {
	this->logicalClock = logicalClock_;
}

long Shard::getLogicalClock() {
	return this->logicalClock;
}

void Shard::setVal(long val_) {
	this->val = val_;
}

long Shard::getVal() {
	return this->val;
}

void Shard::setIsRemoteShard(bool flag) {
	this->isRemoteShard = flag;
}

bool Shard::getIsRemoteShard() {
	return this->isRemoteShard;
}
