/*
 * Client.cpp
 *
 *  Created on: Feb 4, 2015
 *      Author: rajatsub
 */

#include "Client.h"

Client::Client(std::string ipAddress_, int portNumber_, std::string operation_, std::string counterName_, std::string fbfEnable_, int increment_, int transId_) {
	this->setCoordinatorIpAddress(ipAddress_);
	this->setCoordinatorPortNumber(portNumber_);
	this->setOperation(operation_);
	this->setCounterName(counterName_);
	this->setFbfEnable(fbfEnable_);
	this->setIncrement(increment_);
	this->setTransId(transId_);
	opTypeMapping.emplace(CREATEMESSAGE, CREATEMAPPING);
	opTypeMapping.emplace(INCREMENTMESSAGE, INCREMENTMAPPING);
	opTypeMapping.emplace(READMESSAGE, READMAPPING);
}

Client::~Client() {
	// TODO Auto-generated destructor stub
}

void Client::setCoordinatorIpAddress(std::string ipAddress_) {
	this->coordinatorIpAddress = ipAddress_;
}

std::string Client::getCoordinatorIpAddress() {
	return this->coordinatorIpAddress;
}

void Client::setCoordinatorPortNumber(int portNumber_) {
	this->coordinatorPortNumber = portNumber_;
}

int Client::getCoordinatorPortNumber() {
	return this->coordinatorPortNumber;
}

void Client::setOperation(std::string operation_) {
	this->operation = operation_;
}

std::string Client::getOperation() {
	return this->operation;
}

void Client::setCounterName(std::string counterName_) {
	this->counterName = counterName_;
}

std::string Client::getCounterName() {
	return this->counterName;
}

void Client::setFbfEnable(std::string fbfEnable_) {
	this->fbfEnable = fbfEnable_;
}

std::string Client::getFbfEnable() {
	return this->fbfEnable;
}

void Client::setIncrement(int increment_) {
	this->increment = increment_;
}

int Client::getIncrement() {
	return this->increment;
}

void Client::setTransId(int transId_) {
	this->transId = transId_;
}

int Client::getTransId() {
	return this->transId;
}

void Client::printUsage() {
	std::cout<<std::endl<<"Usage of CassandraSimulator Client"<<std::endl;
	std::cout<<"./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <create> <counter_name> <fbf/nofbf>"<<std::endl;
	std::cout<<"./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <increment> <counter_name> <increment_value> <transaction_id>\n";
	std::cout<<"./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <read> <counter_name>\n";
	std::cout<<std::endl<<"Operations supported:\n 1) create 2) read 3) increment"<<std::endl;
	std::cout<<"Quit if the input is not in the above format. No CLA checking done"<<std::endl;
	return;
}

void Client::printWelcome() {
	std::cout<<std::endl<<"**************************************************************"<<std::endl;
	std::cout<<"**************************************************************"<<std::endl;
	std::cout<<"\t\tCASSANDRA SIMULATOR CLIENT\t\t"<<std::endl;
	std::cout<<"**************************************************************"<<std::endl;
	std::cout<<"**************************************************************"<<std::endl<<std::endl;

}

bool Client::setUpSocket() {
	this->tcp = socket(AF_INET, SOCK_STREAM, 0);
	if ( FAILURE == this->tcp ) {
		tcp::sendError();
		return false;
	}
	return true;
}

bool Client::clientSenderFunc() {
	int i_rc;
	int numOfBytesSent = 0;
	memset(&(this->coordinatorAddress), 0, sizeof(struct sockaddr_in));
	this->coordinatorAddress.sin_family = AF_INET;
	this->coordinatorAddress.sin_port = htons(this->getCoordinatorPortNumber());
	this->coordinatorAddress.sin_addr.s_addr = inet_addr((this->getCoordinatorIpAddress()).c_str());
	memset(&(this->coordinatorAddress.sin_zero), '\0', 8);
	i_rc = connect(this->tcp, (struct sockaddr *)&(this->coordinatorAddress), sizeof(this->coordinatorAddress));
	if ( i_rc != SUCCESS ) {
		tcp::connectError();
		return false;
	}
	switch(opTypeMapping[this->getOperation()]) {
	case CREATEMAPPING: {
		bool activateFbf = false;
		if (this->getFbfEnable() == FBFENABLE) {
			activateFbf = true;
		}
		else {
			activateFbf = false;
		}
		std::string msgToSend = Message::createMessage(this->getCounterName(), activateFbf);
		std::cout<<"\nMessage being sent: \n"<<msgToSend<<std::endl;
		numOfBytesSent = tcp::sendTCP(this->tcp, msgToSend);
		if (SUCCESS == numOfBytesSent) {
			tcp::sendError();
			return false;
		}
		break;
	} // End of case CREATEMESSAGE
	case INCREMENTMAPPING: {
		std::string msgToSend = Message::incrementMessage(this->getCounterName(), this->getIncrement(), this->getTransId());
		std::cout<<"\nMessage being sent: \n"<<msgToSend<<std::endl;
		numOfBytesSent = tcp::sendTCP(this->tcp, msgToSend);
		if (SUCCESS == numOfBytesSent) {
			tcp::sendError();
			return false;
		}
		break;
	} // End of case INCREMENTMAPPING
	case READMAPPING: {
		std::string msgToSend = Message::readMessage(this->getCounterName());
		std::cout<<"\nMessage being sent: \n"<<msgToSend<<std::endl;
		numOfBytesSent = tcp::sendTCP(this->tcp, msgToSend);
		if (SUCCESS == numOfBytesSent) {
			tcp::sendError();
			return false;
		}
		break;
	} // End of case READMAPPING
	default: {
		std::cout<<std::endl<<"Invalid operation type. Cant send message to coordinator"<<std::endl;
		break;
	} // End of default
	} // End of switch case
	return true;
}

bool Client::clientRecvFunc() {
	std::string recMsg;
	recMsg.clear();
	recMsg = tcp::recvTCP(this->tcp);
	if ( recMsg.size() == SUCCESS ) {
		tcp::recvError();
		return false;
	}
	recMsg.shrink_to_fit();
	Message msgObj;
	msgObj.extractOpCode(recMsg);
	std::cout<<std::endl<<std::endl<<"Message being received: "<<recMsg<<std::endl<<std::endl;
	switch(msgObj.getMessageType()) {
	case CREATE_SUCCESS: {
		std::cout<<std::endl<<"***********"<<std::endl;
		std::cout<<msgObj.getCounterName()<<" CREATED SUCCESSFULLY!"<<std::endl;
		std::cout<<std::endl<<"***********"<<std::endl;
		break;
	} //End of CREATE_SUCCESS
	case ERROR: {
		std::cout<<std::endl<<"***********"<<std::endl;
		std::cout<<"There was and ERROR during operation on "<<msgObj.getCounterName()<<". Try again !!"<<std::endl;
		std::cout<<std::endl<<"***********"<<std::endl;
		break;
	} // End of ERROR
	case INCREMENT_SUCCESS: {
		std::cout<<std::endl<<"***********"<<std::endl;
		std::cout<<msgObj.getCounterName()<<" INCREMENTED SUCCESSFULLY!"<<std::endl;
		std::cout<<std::endl<<"***********"<<std::endl;
		break;
	} //End of INCREMENT_SUCCESS
	case READ_SUCCESS: {
		std::cout<<std::endl<<"***********"<<std::endl;
		std::cout<<msgObj.getCounterName()<<" READ SUCCESSFULL!"<<std::endl;
		std::cout<<"VALUE: "<<msgObj.getFinalReadValue()<<std::endl;
		break;
	} // End of case READ_SUCCESS
	default: {
		std::cout<<std::endl<<"UNKNOW MESSAGE TYPE RECEIVED"<<std::endl;
		break;
	} // End of default
	} // End of switch case
	return true;
}

int main(int argc, char *argv[]) {
	Client::printWelcome();
	if ( argc == 2 ) {
		if ( argv[1] == HELP ) {
			Client::printUsage();
			return SUCCESS;
		}
		else {
			Client::printUsage();
			return FAILURE;
		}
	}
	std::string ipAddress_(argv[1]);
	std::string portNumber_(argv[2]);
	std::string operation_(argv[3]);
	std::string counterName_;
	std::string fbfEnable_;
	int value_ = 0;
	int transId = 0;
	if ( operation_ == CREATEMESSAGE ) {
		std::cout<<"\n======================\n";
		std::cout<<"COUNTER CREATE"<<std::endl;
		std::cout<<"======================\n";
		counterName_ = std::string(argv[4]);
		fbfEnable_ = std::string(argv[5]);
		/*if ( fbfEnable_ != FBFENABLE || fbfEnable_ != FBFDISABLE ) {
			Client::printUsage();
			return FAILURE;
		}*/
		std::cout<<"Counter name: "<<counterName_<<std::endl;
		std::cout<<"FBF enable/disable: "<<fbfEnable_<<std::endl;
	}
	else if ( operation_ == INCREMENTMESSAGE ) {
		std::cout<<"\n======================\n";
		std::cout<<"COUNTER INCREMENT"<<std::endl;
		std::cout<<"======================\n";
		counterName_ = std::string(argv[4]);
		value_ = atoi(argv[5]);
		transId = atoi(argv[6]);
		std::cout<<"Counter name: "<<counterName_<<std::endl;
		std::cout<<"Increment value: "<<value_<<std::endl;
	}
	else if ( operation_ == READMESSAGE ) {
		std::cout<<"\n======================\n";
		std::cout<<"COUNTER INCREMENT"<<std::endl;
		std::cout<<"======================\n";
		counterName_ = std::string(argv[4]);
		std::cout<<"Counter name: "<<counterName_<<std::endl;
	}
	else {
		std::cout<<"Invalid command line arguments"<<std::endl;
		Client::printUsage();
		return FAILURE;
	}
	Client client(ipAddress_, atoi(portNumber_.c_str()), operation_, counterName_, fbfEnable_, value_, transId);
	if ( !client.setUpSocket() ) {
		std::cout<<std::endl<<"Error while opening socket"<<std::endl;
		return FAILURE;
	}
	if ( !client.clientSenderFunc() ) {
		std::cout<<std::endl<<"Error while sending message to coordinator"<<std::endl;
		return FAILURE;
	}
	if ( !client.clientRecvFunc() ) {
		std::cout<<std::endl<<"Error while receiving message from coordinator"<<std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

