/*
 * NodeId.h
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#ifndef NODEID_H_
#define NODEID_H_

#include "Common.h"

class NodeId {
private:
	std::string ipAddress;
	int portNumber;
	size_t hashCode;
public:
	NodeId(std::string ipAddress_, int portNumber_, size_t hashCode);
	virtual ~NodeId();
	void setIpAddress(std::string ipAddress_);
	std::string getIpAddress();
	void setPortNumber(int portNumber_);
	int getPortNumber();
	void setHashCode(size_t hashCode_);
	size_t getHashCode();
	bool operator < (const NodeId& anotherNodeId) const;
};

#endif /* NODEID_H_ */
