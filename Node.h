/*
 * Node.h
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#ifndef NODE_H_
#define NODE_H_

#include "Common.h"
#include "Message.h"
#include "tcp.h"
#include "NodeId.h"
#include "Counter.h"
#include "FBF.h"

class Node {
private:
	std::string ipAddress;
	int portNumber;
	size_t myHashCode;
	bool isLeader;
	int tcp;
	struct sockaddr_in hostAddress;
	std::thread receiver;
	bool runReceiverThread;
	std::thread broadcaster;
	bool runBroadcasterThread;
	std::vector<NodeId> ring;
	std::mutex ringMutex;
	std::mutex memTableMutex;
	std::map<std::string, Counter> memTable;
	std::map<std::string, FBF*> fbfMap;

public:
	Node(std::string ipAddress_, int portNumber, bool isLeader);
	virtual ~Node();
	void setIpAddress(std::string ipAddress_);
	std::string getIpAddress();
	void setPortNumber(int portNumber_);
	int getPortNumber();
	void setIsLeader(bool isLeader_);
	bool getIsLeader();
	void setMyHashCode(size_t hashCode_);
	size_t getMyHashCode();
	size_t getHashCode(std::string ipAddress_, int portNumber_);
	size_t getHashCode(std::string counterName);
	static void printHelp(bool flag=true);
	void initDataStructures();
	bool setUpPorts();
	void printNodeDetails();
	bool joinLeader(std::string leaderIpAddress, int portNumber);
	void spawnThreads();
	void recieveMessages();
	void handleMessage(int *ptrSD);
	bool addNewNodeToRing(std::string nodeIpAddress, int nodePortNumber);
	bool addNewNodeToRing(std::string nodeIpAddress, int nodePortNumber, size_t hashCode);
	void broadcastRing();
	std::hash<std::string> hashFunc;
	void printRing();
	bool sendRing();
	void deserializeRingInMsgAndBuildRing(std::string ringInMsg);
	std::vector<std::tuple<std::string, int, size_t>> findLeader(std::string counterName_);
	bool createCounter(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>>, bool fbfEnable);
	bool incrementCounterLocal(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>>, int increment_, int transId);
	bool incrementCounterRemote(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>>, size_t nodeId, long logical_ts, long value, int transId);
	std::pair<bool, long> readCounter(std::string counterName_);
	bool isThisMyIpAndPort(std::string ipAddress_, int portNumber_);
	bool isThisMyIpAndPortAndHashCode(std::string ipAddress_, int portNumber_, size_t hashCode_);
	long returnSumLogicalTimeStampOfAllLocalShards(std::string coutnerName_);
	long returnSumValueOfAllLocalShards(std::string counterName_);
};

#endif /* NODE_H_ */
