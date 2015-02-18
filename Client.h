/*
 * Client.h
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#ifndef CLIENT_H_
#define CLIENT_H_

#include "Common.h"
#include "tcp.h"
#include "Message.h"

class Client {
private:
	std::string coordinatorIpAddress;
	int coordinatorPortNumber;
	int tcp;
	struct sockaddr_in coordinatorAddress;
	std::string operation;
	std::string counterName;
	std::string fbfEnable;
	std::map<std::string, int> opTypeMapping;
	int increment;
	int transId;
public:
	Client(std::string ipAddress_, int portNumber_, std::string operation_, std::string counterName, std::string fbfEnable, int increment_, int transId_);
	virtual ~Client();
	static void printUsage();
	static void printWelcome();
	void setCoordinatorIpAddress(std::string ipAddress_);
	std::string getCoordinatorIpAddress();
	void setCoordinatorPortNumber(int portNumber_);
	int getCoordinatorPortNumber();
	void setOperation(std::string operation_);
	std::string getOperation();
	void setCounterName(std::string counterName_);
	std::string getCounterName();
	void setFbfEnable(std::string fbfEnable_);
	std::string getFbfEnable();
	void setIncrement(int increment_);
	int getIncrement();
	void setTransId(int transId_);
	int getTransId();
	bool setUpSocket();
	bool clientSenderFunc();
	bool clientRecvFunc();
};

#endif /* CLIENT_H_ */
