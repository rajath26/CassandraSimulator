/*
 * tcp.h
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#ifndef TCP_H_
#define TCP_H_

#include "Common.h"

class tcp {
public:
	tcp();
	virtual ~tcp();
	static void socketError();
	static void bindError();
	static void connectError();
	static int sendTCP(int *sd, std::string buffer);
	static void sendError();
	static std::string recvTCP(int *sd);
	static void recvError();
};

#endif /* TCP_H_ */
