/*
 * NodeId.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#include "NodeId.h"

NodeId::NodeId(std::string ipAddress_, int portNumber_, size_t hashCode_): ipAddress(ipAddress_),
																		   portNumber(portNumber_),
																		   hashCode(hashCode_)  {}

NodeId::~NodeId() {}

void NodeId::setIpAddress(std::string ipAddress_) {
	this->ipAddress = ipAddress_;
}

std::string NodeId::getIpAddress() {
	return this->ipAddress;
}

void NodeId::setPortNumber(int portNumber_) {
	this->portNumber = portNumber_;
}

int NodeId::getPortNumber() {
	return this->portNumber;
}

void NodeId::setHashCode(size_t hashCode_) {
	this->hashCode = hashCode_;
}

size_t NodeId::getHashCode() {
	return this->hashCode;
}

bool NodeId::operator < (const NodeId& anotherNodeId) const {
	return this->hashCode < anotherNodeId.hashCode;
}
