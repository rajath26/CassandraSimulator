/*
 * Message.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#include "Message.h"

Message::Message() {
}

Message::~Message() {
	// TODO Auto-generated destructor stub
}

void Message::setMessageType(messageType msgType_) {
	this->msgType = msgType_;
}

messageType Message::getMessageType() {
	return this->msgType;
}

void Message::setIpAddress(std::string ipAddress_) {
	this->ipAddress = ipAddress_;
}

std::string Message::getIpAddress() {
	return this->ipAddress;
}

void Message::setPortNumber(int portNumber_) {
	this->portNumber = portNumber_;
}

int Message::getPortNumber() {
	return this->portNumber;
}

void Message::setRingInMsg(std::string msg) {
	this->ringInMsg = msg;
}

std::string Message::getRingInMsg() {
	return this->ringInMsg;
}

void Message::setCounterName(std::string counterName_) {
	this->counterName = counterName_;
}

std::string Message::getCounterName() {
	return this->counterName;
}

void Message::setFbfEnableType(BFtype flag) {
	this->fbfEnableType = flag;
}

BFtype Message::getFbfEnableType() {
	return this->fbfEnableType;
}

void Message::setIncrement(int increment_) {
	this->increment = increment_;
}

int Message::getIncrement() {
	return this->increment;
}

void Message::setNodeId(size_t nodeId_) {
	this->nodeId = nodeId_;
}

size_t Message::getNodeId() {
	return this->nodeId;
}

void Message::setLogicalTimestamp(long logical_ts) {
	this->logicalTimestamp = logical_ts;
}

long Message::getLogicalTimestamp() {
	return this->logicalTimestamp;
}

void Message::setValue(long value_) {
	this->value = value_;
}

long Message::getValue() {
	return this->value;
}

void Message::setTransId(int transId_) {
	this->transId = transId_;
}

int Message::getTransId() {
	return this->transId;
}

void Message::setFinalReadValue(long finalReadValue_) {
	this->finalReadValue = finalReadValue_;
}

int Message::getFinalReadValue() {
	return this->finalReadValue;
}

std::string Message::createJoinMessage(std::string ipAddress, int portNumber) {
	std::string message(JOINMESSAGE);
	message += DELIMITER + ipAddress + DELIMITER + std::to_string(portNumber);
	return message;
}

void Message::extractOpCode(std::string recMsg) {
	int start = 0;
	std::cout<<"extract:"<<recMsg<<std::endl;
	// <op_code>::<remaining_message>
	std::vector<std::string> tokens;
	int pos = recMsg.find(DELIMITER);
	if ( pos != std::string::npos ) {
		tokens.emplace_back(recMsg.substr(start, pos));
	}
	// join::ip_address::port_number
	if ( tokens[0] == JOINMESSAGE ) {
		this->setMessageType(JOIN);
		std::string rem = recMsg.substr(pos + 2);
		//std::cout<<std::endl<<"Remaining: "<<rem<<std::endl;
		int npos = rem.find(DELIMITER, 0);
		//std::cout<<npos<<std::endl;
		if ( npos != std::string::npos ) {
			this->setIpAddress(rem.substr(0, npos));
			this->setPortNumber(stoi(rem.substr(npos+2)));
		}
		return;
	}
	// broadcast::<serialized_table>
	// serialized_table:
	// ip_address::port_number::hash_code;;ip_address::port_number::hash_code;;.......
	else if ( tokens[0] == BROADCASTMESSAGE ) {
		this->setMessageType(MEMBERLIST);
		setRingInMsg(recMsg.substr(pos+2));
		return;
	}
	// create::counter_name::fbf/nofbf/rbf
	else if (tokens[0] == CREATEMESSAGE) {
		this->setMessageType(CREATE);
		start = pos + 2;
		size_t pos = recMsg.find(DELIMITER, start);
		std::string counterName_ = recMsg.substr(start, pos-start);
		this->setCounterName(counterName_);
		std::string fbfStatus = recMsg.substr(pos+2);
		if ( fbfStatus == FBFENABLE ) {
			this->setFbfEnableType(fbfType);
		}
		else if ( fbfStatus == RBFENABLE ) {
			this->setFbfEnableType(rbfType);
		}
		else {
			this->setFbfEnableType(noneType);
		}
		return;
	}
	// create_replica::counter_name::fbf/nofbf/rbf
	else if (tokens[0] == CREATEREPLICAMESSAGE) {
		this->setMessageType(CREATE_REPLICA);
		start = pos + 2;
		size_t pos = recMsg.find(DELIMITER, start);
		std::string counterName_ = recMsg.substr(start, pos-start);
		this->setCounterName(counterName_);
		std::string fbfStatus = recMsg.substr(pos+2);
		/*
		std::string counterName_ = recMsg.substr(pos+2);
		this->setCounterName(counterName_);
		std::string fbfStatus = recMsg.substr(pos+2);
		*/
		if ( fbfStatus == FBFENABLE ) {
			this->setFbfEnableType(fbfType);
		}
		else if ( fbfStatus == RBFENABLE ) {
			this->setFbfEnableType(rbfType);
		}
		else {
			this->setFbfEnableType(noneType);
		}
		return;
	}
	// create_success::counter_name
	else if (tokens[0] == CREATESUCCESSMESSAGE) {
		this->setMessageType(CREATE_SUCCESS);
		std::string counterName_ = recMsg.substr(pos+2);
		counterName.shrink_to_fit();
		this->setCounterName(counterName_);
		return;
	}
	// error::counter_name
	else if(tokens[0] == ERRORMESSAGE) {
		this->setMessageType(ERROR);
		std::string counterName_ = recMsg.substr(pos+2);
		this->setCounterName(counterName_);
		return;
	}
	// increment::counter_name::increment::transaction_id
	else if (tokens[0] == INCREMENTMESSAGE) {
		this->setMessageType(INCREMENT);
		std::vector<std::string> tokens1;
		tokens1.clear();
		start = pos + 2;
		size_t pos = recMsg.find(DELIMITER, start);
		while ( pos != std::string::npos ) {
			// 0 - counterName
			// 1 - increment
			// 2 - transaction_id
			tokens1.emplace_back(recMsg.substr(start, pos-start));
			start = pos + 2;
			pos = recMsg.find(DELIMITER, start);
		}
		tokens1.emplace_back(recMsg.substr(start));
		this->setCounterName(tokens1[0]);
		this->setIncrement(atoi(tokens1[1].c_str()));
		this->setTransId(atoi(tokens1[2].c_str()));
	}
	// increment_success::counter_name
	else if (tokens[0] == INCREMENTSUCCESSMESSAGE) {
		this->setMessageType(INCREMENT_SUCCESS);
		std::string counterName_ = recMsg.substr(pos+2);
		this->setCounterName(counterName_);
		return;
	}
	// increment_replica::counter_name::nodeId::logical_ts::value::transId
	else if (tokens[0] == INCREMENTREPLICAMESSAGE) {
		this->setMessageType(INCREMENT_REPLICA);
		std::vector<std::string> tokens1;
		tokens1.clear();
		start = pos + 2;
		size_t pos = recMsg.find(DELIMITER, start);
		while (pos != std::string::npos) {
			// 0 - counter name
			// 1 - nodeId
			// 2 - logical_ts
			// 3 - value
			// 4 - transId
			tokens1.emplace_back(recMsg.substr(start, pos-start));
			start = pos + 2;
			pos = recMsg.find(DELIMITER, start);
		}
		tokens1.emplace_back(recMsg.substr(start));
		this->setCounterName(tokens1[0]);
		this->setNodeId(atol(tokens1[1].c_str()));
		this->setLogicalTimestamp(atol(tokens1[2].c_str()));
		this->setValue(atol(tokens1[3].c_str()));
		this->setTransId(atoi(tokens1[4].c_str()));
		return;
	}
	// read::counter_name
	else if (tokens[0] == READMESSAGE) {
		this->setMessageType(READ);
		this->setCounterName(recMsg.substr(pos+2));
		return;
	}
	// read_success::counter_name::value
	else if (tokens[0] == READSUCCESSMESSAGE) {
		this->setMessageType(READ_SUCCESS);
		start = pos + 2;
		pos = recMsg.find(DELIMITER, start);
		this->setCounterName(recMsg.substr(start, pos-start));
		this->setFinalReadValue(atol(recMsg.substr(pos+2).c_str()));
		return;
	}
	else {
		std::cout<<std::endl<<"Unknown opCode received by Message::extractOpCode"<<std::endl;
		return;
	}
}

std::string Message::createBroadcastMessage(std::string ringInMsg) {
	std::string message(BROADCASTMESSAGE);
	message += DELIMITER + ringInMsg;
	return message;
}

std::string Message::createReplicaMessage(std::string counterName_, BFtype type) {
	std::string message(CREATEREPLICAMESSAGE);
	std::string fbfStatus;
	if (type == fbfType) {
		fbfStatus = FBFENABLE;
	}
	else if (type == rbfType) {
		fbfStatus = RBFENABLE;
	}
	else {
		fbfStatus = FBFDISABLE;
	}
	message += DELIMITER + counterName_ + DELIMITER + fbfStatus;
	return message;
}

std::string Message::createMessage(std::string counterName_, BFtype type) {
	std::string message(CREATEMESSAGE);
	std::string fbfStatus;
	if (type == fbfType) {
		fbfStatus = FBFENABLE;
	}
	else if (type == rbfType) {
		fbfStatus = RBFENABLE;
	}
	else {
		fbfStatus = FBFDISABLE;
	}
	message += DELIMITER + counterName_ + DELIMITER + fbfStatus;
	return message;
}

std::string Message::incrementMessage(std::string counterName_, int increment_, int transId_) {
	std::string message(INCREMENTMESSAGE);
	message += DELIMITER + counterName_ + DELIMITER + std::to_string(increment_) + DELIMITER + std::to_string(transId_);
	std::cout<<"\n In Message::incrementMessage: "<<message<<std::endl;
	return message;
}

std::string Message::incrementReplicaMessage(std::string counterName_, size_t nodeId, long logical_ts, long value, int transId) {
	std::string message(INCREMENTREPLICAMESSAGE);
	message += DELIMITER + counterName_ + DELIMITER + std::to_string(nodeId) + DELIMITER + std::to_string(logical_ts) + DELIMITER + std::to_string(value) + DELIMITER + std::to_string(transId);
	return message;
}

std::string Message::readMessage(std::string counterName_) {
	std::string message(READMESSAGE);
	message += DELIMITER + counterName_;
	return message;
}

std::string Message::createSuccessMessage(std::string counterName_) {
	std::string message(CREATESUCCESSMESSAGE);
	message += DELIMITER + counterName_;
	return message;
}

std::string Message::incrementSuccessMessage(std::string counterName_) {
	std::string message(INCREMENTSUCCESSMESSAGE);
	message += DELIMITER + counterName_;
	return message;
}

std::string Message::readSuccessMessage(std::string counterName_, long value_) {
	std::string message(READSUCCESSMESSAGE);
	message += DELIMITER + counterName_ + DELIMITER + std::to_string(value_);
	return message;
}

std::string Message::errorMessage(std::string counterName_) {
	std::string message(ERRORMESSAGE);
	message += DELIMITER + counterName_;
	return message;
}
