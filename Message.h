/*
 * Message.h
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "Common.h"

class Message {
private:
	messageType msgType;
	std::string ipAddress;
	int portNumber;
	std::string ringInMsg;
	std::string counterName;
	bool fbfEnable;
	int increment;
	size_t nodeId;
	long logicalTimestamp;
	long value;
	int transId;
	long finalReadValue;
public:
	Message();
	virtual ~Message();
	void setMessageType(messageType msgType_);
	messageType getMessageType();
	void setIpAddress(std::string ipAddress_);
	std::string getIpAddress();
	void setPortNumber(int portNumber_);
	int getPortNumber();
	void setRingInMsg(std::string msg);
	std::string getRingInMsg();
	void setCounterName(std::string counterName_);
	std::string getCounterName();
	void setFbfEnable(bool flag);
	bool getFbfEnable();
	void setIncrement(int increment_);
	int getIncrement();
	void setNodeId(size_t nodeId_);
	size_t getNodeId();
	void setLogicalTimestamp(long logicalTimestamp_);
	long getLogicalTimestamp();
	void setValue(long value_);
	long getValue();
	void setTransId(int transId_);
	int getTransId();
	void setFinalReadValue(long finalReadValue_);
	int getFinalReadValue();
	static std::string createJoinMessage(std::string ipAddress, int portNumber);
	void extractOpCode(std::string recMsg);
	static std::string createBroadcastMessage(std::string ringInMsg);
	static std::string createReplicaMessage(std::string counterName_, bool fbfEnable);
	static std::string createMessage(std::string counterName_, bool fbfEnable);
	static std::string createSuccessMessage(std::string counterName_);
	static std::string errorMessage(std::string counterName_);
	static std::string incrementMessage(std::string counterName_, int increment_, int transId_);
	static std::string incrementReplicaMessage(std::string counterName_, size_t nodeId, long logical_ts, long value, int transId_);
	static std::string incrementSuccessMessage(std::string counterName_);
	static std::string readMessage(std::string counterName_);
	static std::string readSuccessMessage(std::string counterName_, long value);
};

#endif /* MESSAGE_H_ */
