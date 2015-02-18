/*
 * Node.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#include "Node.h"

Node::Node(std::string ipAddress_, int portNumber_, bool isLeader_) {
	setIpAddress(ipAddress_);
	setPortNumber(portNumber_);
	setIsLeader(isLeader_);
	runReceiverThread = false;
	runBroadcasterThread = false;
	initDataStructures();
	if ( this->getIsLeader() ) {
		this->addNewNodeToRing(this->getIpAddress(), this->getPortNumber());
		this->printNodeDetails();
	}
}

Node::~Node() {
	runReceiverThread = false;
	if (this->getIsLeader()) {
		runBroadcasterThread = false;
	}
}

void Node::setIpAddress(std::string ipAddress_) {
	this->ipAddress = ipAddress_;
}

std::string Node::getIpAddress() {
	return this->ipAddress;
}

void Node::setPortNumber(int portNumber_) {
	this->portNumber = portNumber_;
}

int Node::getPortNumber() {
	return this->portNumber;
}

void Node::setIsLeader(bool isLeader_) {
	this->isLeader = isLeader_;
}

bool Node::getIsLeader() {
	return this->isLeader;
}

void Node::setMyHashCode(size_t hashCode_) {
	this->myHashCode = hashCode_;
}

size_t Node::getMyHashCode() {
	return this->myHashCode;
}

size_t Node::getHashCode(std::string ipAddress_, int portNumber_) {
	srand(time(NULL));
	std::string str = ipAddress_ + std::to_string(portNumber) + std::to_string(rand()) + std::to_string(rand());
	return hashFunc(str)%RING_SIZE;
}

size_t Node::getHashCode(std::string counterName_) {
	return hashFunc(counterName_)%RING_SIZE;
}

void Node::printHelp(bool flag) {
	if ( flag ) {
		std::cout<<std::endl<<"Invalid command line arguments"<<std::endl;
	}
	std::cout<<"./<program_name> help"<<std::endl;
	std::cout<<"OR"<<std::endl;
	std::cout<<"./<program_name> \"leader\"/\"node\" <ip_address> <port_number> <leaderAddress> <leaderPortNumber>"<<std::endl;
}

void Node::initDataStructures() {
	ring.clear();
	memTable.clear();
}

void Node::printNodeDetails() {
	std::cout<<std::endl<<std::endl<<"********************************"<<std::endl;
	std::cout<<"MY IP ADDRESS: "<<this->getIpAddress()<<std::endl;
	std::cout<<"MY PORT NUMBER: "<<this->getPortNumber()<<std::endl;
	if (getIsLeader()) {
		std::cout<<"I AM A LEADER NODE"<<std::endl;
	}
	else {
		std::cout<<"I AM A PEER NODE"<<std::endl;
	}
	std::cout<<"MY HASH CODE: "<<this->getMyHashCode()<<std::endl;
	std::cout<<"********************************"<<std::endl<<std::endl;
}

bool Node::setUpPorts() {
	int i_rc = 0;
	tcp = socket(AF_INET, SOCK_STREAM, 0);
	if ( FAILURE == tcp ) {
		tcp::socketError();
		return false;
	}
	std::cout<<"Opened TCP socket"<<std::endl;
	memset(&(this->hostAddress), 0, sizeof(struct sockaddr_in));
	this->hostAddress.sin_family = AF_INET;
	this->hostAddress.sin_port = htons(this->getPortNumber());
	this->hostAddress.sin_addr.s_addr = inet_addr((this->getIpAddress()).c_str());
	memset(&((this->hostAddress).sin_zero), '\0', 8);
	i_rc = ::bind(tcp, (struct sockaddr *) &(this->hostAddress), sizeof(this->hostAddress));
	if ( i_rc < SUCCESS ) {
		tcp::bindError();
		return false;
	}
	std::cout<<"Bind TCP socket"<<std::endl;
	return true;
}

bool Node::joinLeader(std::string leaderIpAddress_, int leaderPortNumber_) {
	int i_rc;
	int leaderSocket = 0;
	int numOfBytesSent = 0;
	struct sockaddr_in leaderAddress;
	leaderSocket = socket(AF_INET, SOCK_STREAM, 0);
	if ( FAILURE == leaderSocket ) {
		tcp::socketError();
		return false;
	}
	memset(&leaderAddress, 0, sizeof(struct sockaddr_in));
	leaderAddress.sin_family = AF_INET;
	std::cout<<std::endl<<"Leader IP Address: "<<leaderIpAddress_.c_str()<<std::endl;
	std::cout<<std::endl<<"Leader port number: "<<leaderPortNumber_<<std::endl;
	leaderAddress.sin_port = htons(leaderPortNumber_);
	leaderAddress.sin_addr.s_addr = inet_addr(leaderIpAddress_.c_str());
	i_rc = connect(leaderSocket, (struct sockaddr *)&leaderAddress, sizeof(leaderAddress));
	if ( SUCCESS != i_rc ) {
		tcp::connectError();
		close(leaderSocket);
		return false;
	}
	std::cout<<"Connected successfully to leader node"<<std::endl;
	std::string messageToBeSent = Message::createJoinMessage(this->ipAddress, this->portNumber);
	std::cout<<std::endl<<"Message to be sent to leader: "<<messageToBeSent<<std::endl;
	numOfBytesSent = tcp::sendTCP(leaderSocket, messageToBeSent);
	if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
		tcp::sendError();
		close(leaderSocket);
		return false;
	}
	std::cout<<"Successfully sent message to the leader"<<std::endl;
	close(leaderSocket);
	return true;
}

void Node::spawnThreads() {
	runReceiverThread = true;
	receiver = std::thread(&Node::recieveMessages, this);
	/*
	 * If you are the leader node, spawn an extra thread that would take an input
	 * from the user and broadcast the membership list as soon as all nodes have
	 * finished joining
	 */
	if ( this->getIsLeader() ) {
		runBroadcasterThread = true;
		broadcaster = std::thread(&Node::broadcastRing, this);
	}
	receiver.join();
	if ( this->getIsLeader() ) {
		broadcaster.join();
	}
	return;
}

void Node::recieveMessages() {
	std::cout<<std::endl<<"########################"<<std::endl;
	std::cout<<"Running receiver thread: "<<std::endl;
	std::cout<<"########################"<<std::endl;
	struct sockaddr_in recvAddress;
	socklen_t len;
	std::vector<std::thread> handlers;
	while (runReceiverThread) {
		listen(this->tcp, LISTEN_QUEUE_LENGTH);
		while (runReceiverThread) {
			int *ptrSD;
			int clientSD;
			ptrSD = new int;
			len = sizeof(recvAddress);
			clientSD = accept(this->tcp, (struct sockaddr *)&recvAddress, &len);
			if ( clientSD < SUCCESS ) {
				 if ( EINTR == errno ) {
					 std::cout<<std::endl<<"Interrupted system call??"<<std::endl;
				     continue;
				 }
			}
			*ptrSD = clientSD;
			std::cout<<std::endl<<"Successfully accepted socket"<<std::endl;
			handlers.emplace_back(std::thread(&Node::handleMessage, this, ptrSD));
		} // End of inner while
	} // End of outer while
	for ( auto& th: handlers ) {
		th.join();
	}
}

void Node::handleMessage(int *ptrSD) {
	Message msgObj;
	int numOfBytesRec = 0;
	int clientSD = *ptrSD;
	std::string recMsg;
	std::cout<<std::endl<<"@@@@@@@@@@@@@@"<<std::endl;
	std::cout<<"Entering thread function"<<std::endl;
	std::cout<<"@@@@@@@@@@@@@@"<<std::endl;
	/*
	 * Step 1: Receive message
	 */
	recMsg = tcp::recvTCP(clientSD);
	numOfBytesRec = recMsg.size();
	if ( SUCCESS == numOfBytesRec || FAILURE == numOfBytesRec ) {
		tcp::recvError();
		return;
	}
	std::cout<<"\t\t Messages received is: \t"<<std::endl<<recMsg<<std::endl;
	/*
	 * Step 2: Extract op code from message and handle it accordingly.
	 */
	msgObj.extractOpCode(recMsg);
	switch(msgObj.getMessageType()) {
	case JOIN: {
		if ( !this->addNewNodeToRing(msgObj.getIpAddress(), msgObj.getPortNumber()) ) {
			std::cout<<std::endl<<"Error while adding new node to the ring. Node IP Address: "<<msgObj.getIpAddress()<<" . Node port number: "<<msgObj.getPortNumber()<<std::endl;
		}
		break;
	} // End of case JOIN
	case MEMBERLIST: {
		/*
		 * Deserialize the ring message and build your own ring
		 */
		std::cout<<"RING IN MESSAGE RECEIVED: "<<msgObj.getRingInMsg()<<std::endl;
		deserializeRingInMsgAndBuildRing(msgObj.getRingInMsg());
		break;
	} // End of case MEMBERLIST
	case CREATE: {
		int successfulResponses = 0;
		std::string counterName_ = msgObj.getCounterName();
		bool fbfEnable_ = msgObj.getFbfEnable();
		// Find the owner/leader node of the counter
		std::vector<std::tuple<std::string, int, size_t>> local;
		local = findLeader(counterName_);
		std::cout<<std::endl<<"Printing the replicas of this key here:"<<std::endl;
		for ( int i = 0; i < local.size(); i++ ) {
			std::cout<<std::get<0>(local[i])<<"\t"<<std::get<1>(local[i])<<"\t"<<std::get<2>(local[i])<<std::endl;
		}
		//
		//If the local node is the leader for this counter
		//
		if ( isThisMyIpAndPortAndHashCode(std::get<0>(local[0]), std::get<1>(local[0]), std::get<2>(local[0])) ) {
			std::cout<<std::endl<<"This message is a local create message"<<std::endl;
			if ( !this->createCounter(counterName_, local, fbfEnable_) ) {
				std::cout<<std::endl<<"There was an error while creating the counter at leaader node. Node IP address: "<<std::get<0>(local[0])<<". Node port number: "<<std::get<1>(local[0])<<". Node hash code: "<<std::get<2>(local[0])<<std::endl;
			}
			else {
				successfulResponses++;
			}
			//
			// Send replication messages to replicas
			//
			std::string msgToBeSent = Message::createReplicaMessage(counterName_, fbfEnable_);
			std::cout<<std::endl<<"Message to be sent to replicas: "<<msgToBeSent<<std::endl;
			for ( int i = 1; i < local.size(); i++ ) {
				int i_rc;
				int replicaSocket = 0;
				int numOfBytesSent = 0;
				std::string msgRecv;
				struct sockaddr_in replicaAddr;
				replicaSocket = socket(AF_INET, SOCK_STREAM, 0);
				if ( FAILURE == replicaSocket ) {
					tcp::socketError();
					//break;
				}
				memset(&replicaAddr, 0, sizeof(struct sockaddr_in));
				replicaAddr.sin_family = AF_INET;
				replicaAddr.sin_port = htons(std::get<1>(local[i]));
				replicaAddr.sin_addr.s_addr = inet_addr((std::get<0>(local[i])).c_str());
				i_rc = connect(replicaSocket, (struct sockaddr *)&replicaAddr, sizeof(replicaAddr));
				if ( SUCCESS != i_rc ) {
					tcp::connectError();
					close(replicaSocket);
					//break;
				}
				std::cout<<"Connected successfully to replica node"<<std::endl;
				numOfBytesSent = tcp::sendTCP(replicaSocket, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(replicaSocket);
					//break;
				}
				std::cout<<"Successfully sent message to the replica"<<std::endl;
				msgRecv = tcp::recvTCP(replicaSocket);
				if ( SUCCESS == msgRecv.size() || FAILURE == msgRecv.size() ) {
					tcp::recvError();
					close(replicaSocket);
					//break;
				}
				successfulResponses++;
				close(replicaSocket);
			}
			std::cout<<std::endl<<"Number of successful replications of CREATE: "<<counterName_<<" is: "<<successfulResponses<<std::endl;
			// Send success or error message back to the client
			if ( successfulResponses >= CONSISTENCY_LEVEL ) {
				std::cout<<"\n--------------------------------------\n";
				std::cout<<"COUNTER CREATED SUCCESSFULLY";
				std::cout<<"\n--------------------------------------\n";
				std::string msgToBeSent = Message::createSuccessMessage(counterName_);
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
			else {
				std::cout<<std::endl<<"Error ! Only "<<successfulResponses<<" number of replicas created"<<std::endl;
				std::string msgToBeSent = Message::errorMessage(counterName_);
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
		} // End of if local
		// Else route the incoming message to the peer
		else {
			std::cout<<std::endl<<"This message needs to be routed to peer"<<std::endl;
			int i_rc;
			int peerLeaderSocket = 0;
			int numOfBytesSent = 0;
			struct sockaddr_in peerLeaderAddress;
			peerLeaderSocket = socket(AF_INET, SOCK_STREAM, 0);
			if ( FAILURE == peerLeaderSocket ) {
				tcp::socketError();
				//break;
			}
			memset(&peerLeaderAddress, 0, sizeof(struct sockaddr_in));
			peerLeaderAddress.sin_family = AF_INET;
			peerLeaderAddress.sin_port = htons(std::get<1>(local[0]));
			peerLeaderAddress.sin_addr.s_addr = inet_addr((std::get<0>(local[0])).c_str());
			i_rc = connect(peerLeaderSocket, (struct sockaddr *)&peerLeaderAddress, sizeof(peerLeaderAddress));
			if ( SUCCESS != i_rc ) {
				tcp::connectError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Connected successfully to leader node during peer routing"<<std::endl;
			// send a message to the peer leader
			numOfBytesSent = tcp::sendTCP(peerLeaderSocket, recMsg);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Successfully sent message to the leader during peer routing"<<std::endl;
			// get a response from the peer leader
			std::string response = tcp::recvTCP(peerLeaderSocket);
			if ( SUCCESS == response.size() || FAILURE == response.size() ) {
				tcp::recvError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Got response from leader during peer routing"<<std::endl;
			// send a message back to the client
			numOfBytesSent = tcp::sendTCP(clientSD, response);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				//break;
			}
			close(peerLeaderSocket);
			close(clientSD);
		} // End of peer routing
		break;
	} // End of case CREATE
	case CREATE_REPLICA: {
		std::string counterName_ = msgObj.getCounterName();
		bool fbfEnable_ = msgObj.getFbfEnable();
		// Find the replicas to create shards
		std::vector<std::tuple<std::string, int, size_t>> local;
		local = findLeader(counterName_);
		if ( !this->createCounter(counterName_, local, fbfEnable_) ) {
			std::cout<<std::endl<<"There was an error while creating the counter at replica node."<<std::endl;
			// send error back to leader node
			std::string msgToBeSent = Message::errorMessage(counterName_);
			int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				break;
			}
		}
		else {
			std::string msgToBeSent = Message::createSuccessMessage(counterName_);
			int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				break;
			}
		}
		break;
	} // End of case CREATE_REPLICA
	case INCREMENT: {
		int successfulResponses = 0;
		std::string counterName_ = msgObj.getCounterName();
		int incrementValue_ = msgObj.getIncrement();
		int transId_ = msgObj.getTransId();
		// Find the owner/leader node of the counter
		std::vector<std::tuple<std::string, int, size_t>> local;
		local = findLeader(counterName_);
		std::cout<<std::endl<<"Printing the replicas of this key here:"<<std::endl;
		for ( int i = 0; i < local.size(); i++ ) {
			std::cout<<std::get<0>(local[i])<<"\t"<<std::get<1>(local[i])<<"\t"<<std::get<2>(local[i])<<std::endl;
		}
		//
		//If the local node is the leader for this counter
		//
		if ( isThisMyIpAndPortAndHashCode(std::get<0>(local[0]), std::get<1>(local[0]), std::get<2>(local[0])) ) {
			std::cout<<std::endl<<"This message is a local increment message"<<std::endl;
			if ( !this->incrementCounterLocal(counterName_, local, incrementValue_, transId_) ) {
				std::cout<<std::endl<<"There was an error while incrementing the counter at leaader node. Node IP address: "<<std::get<0>(local[0])<<". Node port number: "<<std::get<1>(local[0])<<". Node hash code: "<<std::get<2>(local[0])<<std::endl;
			}
			else {
				successfulResponses++;
			}
			//
			// Send replication messages to replicas
			//
			std::string msgToBeSent = Message::incrementReplicaMessage(counterName_, this->getMyHashCode(), this->returnSumLogicalTimeStampOfAllLocalShards(counterName_), this->returnSumValueOfAllLocalShards(counterName_), transId_);
			std::cout<<std::endl<<"Message to be sent to replicas: "<<msgToBeSent<<std::endl;
			for ( int i = 1; i < local.size(); i++ ) {
				int i_rc;
				int replicaSocket = 0;
				int numOfBytesSent = 0;
				std::string msgRecv;
				struct sockaddr_in replicaAddr;
				replicaSocket = socket(AF_INET, SOCK_STREAM, 0);
				if ( FAILURE == replicaSocket ) {
					tcp::socketError();
					//break;
				}
				memset(&replicaAddr, 0, sizeof(struct sockaddr_in));
				replicaAddr.sin_family = AF_INET;
				replicaAddr.sin_port = htons(std::get<1>(local[i]));
				replicaAddr.sin_addr.s_addr = inet_addr((std::get<0>(local[i])).c_str());
				i_rc = connect(replicaSocket, (struct sockaddr *)&replicaAddr, sizeof(replicaAddr));
				if ( SUCCESS != i_rc ) {
					tcp::connectError();
					close(replicaSocket);
					//break;
				}
				std::cout<<"Connected successfully to replica node"<<std::endl;
				numOfBytesSent = tcp::sendTCP(replicaSocket, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(replicaSocket);
					//break;
				}
				std::cout<<"Successfully sent message to the replica"<<std::endl;
				msgRecv = tcp::recvTCP(replicaSocket);
				if ( SUCCESS == msgRecv.size() || FAILURE == msgRecv.size() ) {
					tcp::recvError();
					close(replicaSocket);
					//break;
				}
				successfulResponses++;
				close(replicaSocket);
			}
			std::cout<<std::endl<<"Number of successful replications of INCREMENT: "<<counterName_<<" is: "<<successfulResponses<<std::endl;
			// Send success or error message back to the client
			if ( successfulResponses >= CONSISTENCY_LEVEL ) {
				std::cout<<"\n--------------------------------------\n";
				std::cout<<"COUNTER INCREMENTED SUCCESSFULLY";
				std::cout<<"\n--------------------------------------\n";
				std::string msgToBeSent = Message::incrementSuccessMessage(counterName_);
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
			else {
				std::string msgToBeSent = Message::errorMessage(counterName_);
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
		} // End of if local
		// Else route the incoming message to the peer
		else {
			std::cout<<std::endl<<"This message needs to be routed to peer"<<std::endl;
			int i_rc;
			int peerLeaderSocket = 0;
			int numOfBytesSent = 0;
			struct sockaddr_in peerLeaderAddress;
			peerLeaderSocket = socket(AF_INET, SOCK_STREAM, 0);
			if ( FAILURE == peerLeaderSocket ) {
				tcp::socketError();
				//break;
			}
			memset(&peerLeaderAddress, 0, sizeof(struct sockaddr_in));
			peerLeaderAddress.sin_family = AF_INET;
			peerLeaderAddress.sin_port = htons(std::get<1>(local[0]));
			peerLeaderAddress.sin_addr.s_addr = inet_addr((std::get<0>(local[0])).c_str());
			i_rc = connect(peerLeaderSocket, (struct sockaddr *)&peerLeaderAddress, sizeof(peerLeaderAddress));
			if ( SUCCESS != i_rc ) {
				tcp::connectError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Connected successfully to leader node during peer routing"<<std::endl;
			// send a message to the peer leader
			numOfBytesSent = tcp::sendTCP(peerLeaderSocket, recMsg);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Successfully sent message to the leader during peer routing"<<std::endl;
			// get a response from the peer leader
			std::string response = tcp::recvTCP(peerLeaderSocket);
			if ( SUCCESS == response.size() || FAILURE == response.size() ) {
				tcp::recvError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Got response from leader during peer routing"<<std::endl;
			// send a message back to the client
			numOfBytesSent = tcp::sendTCP(clientSD, response);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				//break;
			}
			close(peerLeaderSocket);
			close(clientSD);
		} // End of peer routing
		break;
	} // End of CASE INCREMENT
	case INCREMENT_REPLICA: {
		std::string counterName_ = msgObj.getCounterName();
		std::vector<std::tuple<std::string, int, size_t>> local;
		local = findLeader(counterName_);
		std::cout<<"\nThe value is : "<<msgObj.getValue()<<"\n";
		if ( !this->incrementCounterRemote(counterName_, local, msgObj.getNodeId(), msgObj.getLogicalTimestamp(), msgObj.getValue(), msgObj.getTransId()) ) {
			std::cout<<std::endl<<"There was an error while incrementing the counter at replica node."<<std::endl;
			// send error back to leader node
			std::string msgToBeSent = Message::errorMessage(counterName_);
			int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				break;
			}
		}
		else {
			std::string msgToBeSent = Message::incrementSuccessMessage(counterName_);
			int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				break;
			}
		}
		break;
	} // End of case INCREMENT_REPLICA
	case READ: {
		std::string counterName_ = msgObj.getCounterName();
		// Find the owner/leader node of the counter
		std::vector<std::tuple<std::string, int, size_t>> local;
		local = findLeader(counterName_);
		std::cout<<std::endl<<"Printing the replicas of this key here:"<<std::endl;
		for ( int i = 0; i < local.size(); i++ ) {
			std::cout<<std::get<0>(local[i])<<"\t"<<std::get<1>(local[i])<<"\t"<<std::get<2>(local[i])<<std::endl;
		}
		//
		//If the local node is the leader for this counter
		//
		bool amIReplica = false;
		for ( auto it: local ) {
			if ( isThisMyIpAndPortAndHashCode(std::get<0>(it), std::get<1>(it), std::get<2>(it)) ) {
				amIReplica = true;
			}
		}
		if ( amIReplica ) {
			std::cout<<std::endl<<"This message is a local read message"<<std::endl;
			long retValue = 0;
			auto ret = this->readCounter(counterName_);
			retValue = ret.second;
			//std::cout<<"\nRet value obtained: "<<retValue<<std::endl;
			// Send success or error message back to the client
			if ( ret.first ) {
				std::cout<<"\n--------------------------------------\n";
				std::cout<<"COUNTER READ SUCCESSFULL";
				std::cout<<"\n--------------------------------------\n";
				std::string msgToBeSent = Message::readSuccessMessage(counterName_, retValue);
				//std::cout<<"\nMessage being sent is: \t"<<msgToBeSent<<"\n\n";
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
			else {
				std::cout<<std::endl<<"Error ! Cant read counter value ! "<<std::endl;
				std::string msgToBeSent = Message::errorMessage(counterName_);
				int numOfBytesSent = tcp::sendTCP(clientSD, msgToBeSent);
				if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
					tcp::sendError();
					close(clientSD);
					//break;
				}
			}
		} // End of if local
		// Else route the incoming message to the peer
		else {
			std::cout<<std::endl<<"This message needs to be routed to peer"<<std::endl;
			int i_rc;
			int peerLeaderSocket = 0;
			int numOfBytesSent = 0;
			struct sockaddr_in peerLeaderAddress;
			peerLeaderSocket = socket(AF_INET, SOCK_STREAM, 0);
			if ( FAILURE == peerLeaderSocket ) {
				tcp::socketError();
				//break;
			}
			memset(&peerLeaderAddress, 0, sizeof(struct sockaddr_in));
			peerLeaderAddress.sin_family = AF_INET;
			peerLeaderAddress.sin_port = htons(std::get<1>(local[0]));
			peerLeaderAddress.sin_addr.s_addr = inet_addr((std::get<0>(local[0])).c_str());
			i_rc = connect(peerLeaderSocket, (struct sockaddr *)&peerLeaderAddress, sizeof(peerLeaderAddress));
			if ( SUCCESS != i_rc ) {
				tcp::connectError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Connected successfully to leader node during peer routing"<<std::endl;
			// send a message to the peer leader
			numOfBytesSent = tcp::sendTCP(peerLeaderSocket, recMsg);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Successfully sent message to the leader during peer routing"<<std::endl;
			// get a response from the peer leader
			std::string response = tcp::recvTCP(peerLeaderSocket);
			if ( SUCCESS == response.size() || FAILURE == response.size() ) {
				tcp::recvError();
				close(peerLeaderSocket);
				//break;
			}
			std::cout<<"Got response from leader during peer routing"<<std::endl;
			// send a message back to the client
			numOfBytesSent = tcp::sendTCP(clientSD, response);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(clientSD);
				//break;
			}
			close(peerLeaderSocket);
			close(clientSD);
		} // End of peer routing
		break;
	} // End of case READ
	default:
		std::cout<<std::endl<<"Received unknown message type. Node IP Address: "<<msgObj.getIpAddress()<<" . Node port number: "<<msgObj.getPortNumber()<<std::endl;
		break;
	}
	return;
}

bool Node::addNewNodeToRing(std::string nodeIpAddress, int nodePortNumber) {
	std::lock_guard<std::mutex> guard(ringMutex);
	std::cout<<std::endl<<"Node being added to the ring: "<<std::endl;
	std::cout<<"IP ADDRESS: "<<nodeIpAddress<<std::endl;
	std::cout<<"PORT NUMBER: "<<nodePortNumber<<std::endl;
	size_t hashCode_ = getHashCode(nodeIpAddress, nodePortNumber);
	NodeId thisNode(nodeIpAddress, nodePortNumber, hashCode_);
	ring.emplace_back(thisNode);
	std::sort(ring.begin(), ring.end());
	std::cout<<std::endl<<"Successfully added the node to the ring"<<std::endl;
	this->printRing();
	if ( this->getIsLeader() && isThisMyIpAndPort(nodeIpAddress, nodePortNumber) ) {
		this->setMyHashCode(hashCode_);
	}
	return true;
}

bool Node::addNewNodeToRing(std::string nodeIpAddress, int nodePortNumber, size_t hashCode) {
	std::lock_guard<std::mutex> guard(ringMutex);
	std::cout<<std::endl<<"Node being added to the ring: "<<std::endl;
	std::cout<<"IP ADDRESS: "<<nodeIpAddress<<std::endl;
	std::cout<<"PORT NUMBER: "<<nodePortNumber<<std::endl;
	std::cout<<"HASH CODE: "<<hashCode<<std::endl;
	NodeId thisNode(nodeIpAddress, nodePortNumber, hashCode);
	ring.emplace_back(thisNode);
	std::sort(ring.begin(), ring.end());
	std::cout<<std::endl<<"Successfully added the node to the ring"<<std::endl;
	this->printRing();
	if (!this->getIsLeader() && isThisMyIpAndPort(nodeIpAddress, nodePortNumber)) {
		this->setMyHashCode(hashCode);
		this->printNodeDetails();
	}
	return true;
}

void Node::broadcastRing() {
	std::string input;
	while (runBroadcasterThread) {
		std::this_thread::sleep_for(std::chrono::seconds(BROADCAST_MESSAGE_INTERVAL));
		std::cout<<std::endl<<"Type \"DONE\" below to broadcast the ring from the leader to all the participating nodes: "<<std::endl;
		std::cin>>input;
		if ( input == DONE ) {
			std::cout<<std::endl<<"All nodes have joined and you have typed \"DONE\""<<std::endl;
			runBroadcasterThread = false;
			if ( !sendRing() ) {
				std::cout<<std::endl<<"There was an error while broadcasting the ring"<<std::endl;
			}
			break;
		}
	}
	return;
}

bool Node::sendRing() {
	std::lock_guard<std::mutex> guard(ringMutex);
	int i_rc;
	std::string ringInMsg;
	ringInMsg.clear();
	for ( int counter = 0; counter < ring.size(); counter++ ) {
		ringInMsg += ring.at(counter).getIpAddress() + DELIMITER + std::to_string(ring.at(counter).getPortNumber()) + DELIMITER + std::to_string(ring.at(counter).getHashCode()) + ENTRY_END;
	}
	std::string msgToBeSent = Message::createBroadcastMessage(ringInMsg);
	std::cout<<std::endl<<"Message to be broadcast to peer nodes: "<<msgToBeSent<<std::endl;
	for ( int counter = 0; counter < ring.size(); counter++ ) {
		if ( ring.at(counter).getHashCode() != getMyHashCode() ) {
			int peerSocket = 0;
			struct sockaddr_in peerAddress;
			int numOfBytesSent = 0;
			peerSocket = socket(AF_INET, SOCK_STREAM, 0);
			if ( FAILURE == peerSocket ) {
				tcp::socketError();
				return false;
			}
			std::cout<<std::endl<<"Socket to peer successful"<<std::endl;
			memset(&peerAddress, 0, sizeof(struct sockaddr_in));
			peerAddress.sin_family = AF_INET;
			peerAddress.sin_port = htons(ring.at(counter).getPortNumber());
			peerAddress.sin_addr.s_addr = inet_addr(ring.at(counter).getIpAddress().c_str());
			i_rc = connect(peerSocket, (struct sockaddr *)&peerAddress, sizeof(peerAddress));
			if ( SUCCESS != i_rc ) {
				tcp::connectError();
				close(peerSocket);
				return false;
			}
			std::cout<<"Connected successfully to peer node"<<std::endl;
			numOfBytesSent = tcp::sendTCP(peerSocket, msgToBeSent);
			if ( SUCCESS == numOfBytesSent || FAILURE == numOfBytesSent ) {
				tcp::sendError();
				close(peerSocket);
				return false;
			}
			std::cout<<"Successfully sent message to the peer"<<std::endl;
			close(peerSocket);
		} // End of if
	} // End of for
	return true;
}

void Node::printRing() {
	std::cout<<std::endl<<"THIS IS THE CASSANDRA SIMULATOR RING"<<std::endl;
	std::cout<<"*********************************************************"<<std::endl;
	std::cout<<"IP ADDRESS\t\tPORT NUMBER\t\tHASH CODE"<<std::endl;
	std::cout<<"*********************************************************"<<std::endl;
	for ( int counter = 0; counter < ring.size(); counter++ ) {
		std::cout<<ring.at(counter).getIpAddress()<<"\t\t"<<ring.at(counter).getPortNumber()<<"\t\t"<<ring.at(counter).getHashCode()<<std::endl;
	}
	std::cout<<"*********************************************************"<<std::endl;
	std::cout<<"*********************************************************"<<std::endl;
}

void Node::deserializeRingInMsgAndBuildRing(std::string ringInMsg) {
	std::string entryEnd(ENTRY_END);
	std::string delimiters(DELIMITER);
	std::vector<std::string> entries;
	entries.clear();
	size_t pos = ringInMsg.find(entryEnd);
	size_t start = 0;
	while (pos != std::string::npos ) {
		std::string field = ringInMsg.substr(start, pos-start);
		entries.emplace_back(field);
		start = pos + 2;
		pos = ringInMsg.find(entryEnd, start);
	}
	for ( int counter = 0; counter < entries.size(); counter++ ) {
		std::vector<std::string> individualEntries;
		size_t pos = entries.at(counter).find(delimiters);
		size_t start = 0;
		while ( pos != std::string::npos ) {
			std::string field = entries.at(counter).substr(start, pos-start);
			individualEntries.emplace_back(field);
			start = pos + 2;
			pos = entries.at(counter).find(DELIMITER, start);
		}
		individualEntries.emplace_back(entries.at(counter).substr(start));
		this->addNewNodeToRing(individualEntries[0], atoi(individualEntries[1].c_str()), atoi(individualEntries[2].c_str()));
	}
	return;
}

std::vector<std::tuple<std::string, int, size_t>> Node::findLeader(std::string counterName_) {
	std::vector<std::tuple<std::string, int, size_t>> ret;
	if ( ring.size() >= REPLICATION_FACTOR ) {
		size_t hashCode_ = getHashCode(counterName_);
		std::cout<<std::endl<<"In find leader, the hash code of the passed in key: "<<counterName_<<" is: "<<hashCode_<<std::endl;
		if ( hashCode_ <= ring.at(0).getHashCode() || hashCode_ > ring.at(ring.size()-1).getHashCode() ) {
			ret.emplace_back(std::make_tuple(ring.at(0).getIpAddress(), ring.at(0).getPortNumber(), ring.at(0).getHashCode()));
			ret.emplace_back(std::make_tuple(ring.at(1).getIpAddress(), ring.at(1).getPortNumber(), ring.at(1).getHashCode()));
			ret.emplace_back(std::make_tuple(ring.at(2).getIpAddress(), ring.at(2).getPortNumber(), ring.at(2).getHashCode()));
		}
		else {
			for ( int i = 0; i < ring.size(); i++ ) {
				if ( hashCode_ <= ring.at(i).getHashCode() ) {
					ret.emplace_back(std::make_tuple(ring.at(i).getIpAddress(), ring.at(i).getPortNumber(), ring.at(i).getHashCode()));
					ret.emplace_back(std::make_tuple(ring.at((i+1)%ring.size()).getIpAddress(), ring.at((i+1)%ring.size()).getPortNumber(), ring.at((i+1)%ring.size()).getHashCode()));
					ret.emplace_back(std::make_tuple(ring.at((i+2)%ring.size()).getIpAddress(), ring.at((i+2)%ring.size()).getPortNumber(), ring.at((i+2)%ring.size()).getHashCode()));
					break;
				}
			}
		}
		return ret;
	}
}

bool Node::createCounter(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>> replicas, bool fbfEnable) {
	std::lock_guard<std::mutex> lock(this->memTableMutex);
	Counter counterObj(fbfEnable);
	memTable.emplace(counterName_, counterObj);
	if ( counterObj.getFBFenable() ) {
		FBF * fbfObj = new FBF(MINIMUM_NUMBER_OF_BFS, 25000, 5, 30, 0.001, true);
		fbfMap[counterName_] = fbfObj;
	}
	return true;
}

bool Node::incrementCounterLocal(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>>, int increment_, int transId) {
	std::cout<<"\n\nThe transaction id is: "<<transId<<"\n";
	if ( (&memTable[counterName_])->getFBFenable() ) {
		// FBF enabled
		if ( fbfMap[counterName_]->membershipCheck(transId) ) {
			// If found in FBF, this transaction is already completed
			std::cout<<std::endl<<std::endl<<"THIS IS A DUPLICATE. FBF GUARANTEES IDEMPOTENT UPDATES."<<std::endl<<std::endl;
			return false;
		}
		else {
			// If not found in FBF
			fbfMap[counterName_]->insert(transId);
			std::lock_guard<std::mutex> lock(this->memTableMutex);
			return (&memTable[counterName_])->addLocalShard(this->getMyHashCode(), increment_);
		}
	}
	else {
		// FBF disabled
		std::lock_guard<std::mutex> lock(this->memTableMutex);
		return (&memTable[counterName_])->addLocalShard(this->getMyHashCode(), increment_);
	}
	return true;
}

bool Node::incrementCounterRemote(std::string counterName_, std::vector<std::tuple<std::string, int, size_t>>, size_t nodeId_, long logical_ts, long value_, int transId) {
	std::cout<<"\n\nThe transaction id is: "<<transId<<"\n";
	if ( (&memTable[counterName_])->getFBFenable() ) {
		// FBF enabled
		if ( fbfMap[counterName_]->membershipCheck(transId) ) {
			// If found in FBF, this transaction is already completed
			std::cout<<std::endl<<std::endl<<"THIS IS A DUPLICATE. FBF GUARANTEES IDEMPOTENT UPDATES."<<std::endl<<std::endl;
			return false;
		}
		else {
			// If not found in FBF
			fbfMap[counterName_]->insert(transId);
			std::lock_guard<std::mutex> lock(this->memTableMutex);
			return (&memTable[counterName_])->addRemoteShard(nodeId_, logical_ts, value_);
		}
	}
	else {
		std::lock_guard<std::mutex> lock(this->memTableMutex);
		return (&memTable[counterName_])->addRemoteShard(nodeId_, logical_ts, value_);
	}
}

std::pair<bool, long> Node::readCounter(std::string counterName_) {
	std::lock_guard<std::mutex> lock(this->memTableMutex);
	return (&memTable[counterName_])->readCounterValue();
}

long Node::returnSumLogicalTimeStampOfAllLocalShards(std::string counterName_) {
	std::lock_guard<std::mutex> lock(this->memTableMutex);
	return (&memTable[counterName_])->returnSumOfLogicalTsOfAllLocalShards();
}

long Node::returnSumValueOfAllLocalShards(std::string counterName_) {
	std::lock_guard<std::mutex> lock(this->memTableMutex);
	return (&memTable[counterName_])->returnSumOfAllValuesOfAllLocalShards();
}

bool Node::isThisMyIpAndPort(std::string ipAddress_, int portNumber_) {
	if ( this->getIpAddress() == ipAddress_ && this->getPortNumber() == portNumber_ ) {
		return true;
	}
	else {
		return false;
	}
}

bool Node::isThisMyIpAndPortAndHashCode(std::string ipAddress_, int portNumber_, size_t hashCode_) {
	if ( this->getIpAddress() == ipAddress_ && this->getPortNumber() == portNumber_ && this->getMyHashCode() == hashCode_ ) {
		return true;
	}
	else {
		return false;
	}
}

int main(int argc, char *argv[]) {
	/*
	 * ./<program_name> leader/node <ip_address> <port_number> <leaderAddress> <leaderPortNumber>
	 */
	// help option
	if ( argc == 2 ) {
		if ( argv[1] == HELP ) {
			Node::printHelp(false);
			return SUCCESS;
		}
		else {
			Node::printHelp();
			return FAILURE;
		}
	}
	else if ( ARGC_COUNT != argc ) {
		Node::printHelp();
		return FAILURE;
	}

	std::string argv2(argv[1]);
	bool amILeader = false;

	if ( argv2.compare(LEADERNODE) == 0 ) {
		amILeader = true;
	}
	else if ( argv2.compare(PEERNODE) == 0 ) {
		amILeader = false;
	}
	else {
		Node::printHelp();
		return FAILURE;
	}
	std::string passedInIpAddress(argv[2]);
	int passedInPortNumber = atoi(argv[3]);
	/*
	 * Create the node
	 */
	Node node(passedInIpAddress, passedInPortNumber, amILeader);
	/*
	 * Setup TCP port
	 */
	if (!node.setUpPorts()) {
		std::cout<<"There was an error setting up ports"<<std::endl;
		return FAILURE;
	}
	/*
	 * If this is a peer node then it has join the distributed system
	 */
	if (!node.getIsLeader()) {
		std::string leaderIpAddress(argv[4]);
		int leaderPortNumber = atoi(argv[5]);
		if (!node.joinLeader(leaderIpAddress, leaderPortNumber)) {
			std::cout<<"There was an error while joining the distributed system"<<std::endl;
			return FAILURE;
		}
	}
	/*
	 * Spawn threads
	 */
	node.spawnThreads();
} // End of main


