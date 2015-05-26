/*
 * Common.h
 *
 *  Created on: Feb 3, 2015
 *      Author: rajatsub
 */

#ifndef COMMON_H_
#define COMMON_H_

#include <iostream>
#include <string>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
#include <thread>
#include <vector>
#include <chrono>
#include <algorithm>
#include <mutex>
#include <map>
#include <tuple>
#include <utility>

#define ARGC_COUNT 6
#define FAILURE -1
#define SUCCESS 0
#define LISTEN_QUEUE_LENGTH 10
#define LONG_BUF_SZ 4096
#define BROADCAST_MESSAGE_INTERVAL 5
#define DONE "DONE"
#define RING_SIZE 512
#define JOINMESSAGE "join"
#define BROADCASTMESSAGE "broadcast"
#define CREATEMESSAGE "create"
#define CREATEREPLICAMESSAGE "create_replica"
#define CREATESUCCESSMESSAGE "create_success"
#define INCREMENTMESSAGE "increment"
#define INCREMENTSUCCESSMESSAGE "increment_success"
#define INCREMENTREPLICAMESSAGE "increment_replica"
#define READMESSAGE "read"
#define READSUCCESSMESSAGE "read_success"
#define CREATEMAPPING 1
#define INCREMENTMAPPING 2
#define READMAPPING 3
#define FBFENABLE "fbf"
#define FBFDISABLE "nofbf"
#define ERRORMESSAGE "error"
#define DELIMITER "::"
#define ENTRY_END ";;"
#define REPLICATION_FACTOR 3
#define CONSISTENCY_LEVEL 3
#define HELP "help"
#define LEADERNODE "leader"
#define PEERNODE "node"
#define REMOTESTRING "r"
#define LOCALSTRING "l"

enum messageType {JOIN, MEMBERLIST, CREATE, CREATE_REPLICA, CREATE_SUCCESS, ERROR, INCREMENT, INCREMENT_SUCCESS, INCREMENT_REPLICA, READ, READ_SUCCESS, UNKNOWN};

#endif /* COMMON_H_ */
