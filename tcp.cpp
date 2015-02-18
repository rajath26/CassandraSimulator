/*
 * tcp.cpp
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#include "tcp.h"

tcp::tcp() {
	// TODO Auto-generated constructor stub

}

tcp::~tcp() {
	// TODO Auto-generated destructor stub
}

void tcp::socketError() {
	std::cout<<std::endl<<"Unable to open socket"<<std::endl;
	std::cout<<"Error number: "<<errno<<std::endl;
	perror("socket");
	std::cout<<"Exiting.... ... .. . . ."<<std::endl;
}

void tcp::bindError() {
	std::cout<<std::endl<<"Unable to bind TCP socket"<<std::endl;
	std::cout<<"Error number: "<<errno<<std::endl;
	perror("bind");
	std::cout<<"Exiting.... ... .. . . ."<<std::endl;
}

void tcp::connectError() {
	std::cout<<std::endl<<"Unable to connect"<<std::endl;
	std::cout<<"Error number: "<<errno<<std::endl;
	perror("connect");
	std::cout<<"Exiting.... ... .. . . ."<<std::endl;
}

int tcp::sendTCP(int sd, std::string buffer) {
	int numOfBytesSent;
	numOfBytesSent = send(sd, buffer.c_str(), buffer.size(), 0);
	return numOfBytesSent;
}

void tcp::sendError() {
	std::cout<<std::endl<<"ZERO bytes sent"<<std::endl;
	std::cout<<"Exiting.... ... .. . . ."<<std::endl;
}

std::string tcp::recvTCP(int sd) {
	int numOfBytesRec;
	char buf[LONG_BUF_SZ];
	numOfBytesRec = recv(sd, buf, LONG_BUF_SZ, 0);
	std::string buffer(buf);
	//std::cout<<std::endl<<"Message received in tcp::recvTCP: "<<buffer<<std::endl;
	return buffer;
}

void tcp::recvError() {
	std::cout<<std::endl<<"ZERO bytes received"<<std::endl;
	std::cout<<"Exiting.... ... .. . . ."<<std::endl;
}
