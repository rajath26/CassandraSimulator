-- This is a Distributed Key-Value(KV) store developed in C++ based on the design principles of Apache Casasndra. It follows a client-server architecture. The server can be scaled horizontally by adding multiple nodes/servers. The client supports basic CRUD operations on the data structures defined on the server. Communication among the nodes in the distributed system is via message passing. The only data model the server supports for now is a fast, correct, idempotent, distributed counters. It can be used for keeping count of events, system objects, etc.,.


-- How do I run this code ? 

1) Compile by running make 

2) This creates two main executables CassandraNode and Client

3) CassandraNode is the server-side module and Client is the client-side module

4) Here are the run options of the CassandraNode :

$./CassandraNodee

Invalid command line arguments
./<program_name> help
OR
./<program_name> "leader"/"node" <ip_address> <port_number> <leaderAddress> <leaderPortNumber>

There should be one leader node and rest are peer nodes. 

5) Once you have started all the nodes you want to add to the distributed system, pass the word "DONE" to the leader. This triggers the leader to send the membership list to all the other nodes. T

6) Each node sets up a logical ring based on Consistent Hashing and the keys are partitioned based on Consistent Hashing. 

7) The client run options are : 

./Client help

**************************************************************
**************************************************************
		CASSANDRA SIMULATOR CLIENT
**************************************************************
**************************************************************


Usage of CassandraSimulator Client
./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <create> <counter_name> <fbf/nofbf/rbf>
./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <increment> <counter_name> <increment_value> <transaction_id>
./<program_name> <co_ordinator_ip_address> <co_ordinator_port_no> <read> <counter_name>

Operations supported:
 1) create 2) read 3) increment
Quit if the input is not in the above format. No CLA checking done


-- How do I generate the graphs in the thesis ? 

- Note that parameters are already set in macros in the header file. 

-- Fig. 6.7 and Fig. 6.9

1) Set up a 10 node cluster with 10 servers as described in the thesis in a ring topology

2) Run the following python script to automate the client operations: 

$python automateClient.py
Invalid usage
usage: python automateClient.py <counter_fbf> <counter_nofbf> <counter_rbf> <tries> [<coordinator_ip> <coordinator_port>,....]

3) Give names for counter with FBF, counter without FBF and counter with RBF and number of tries as a number over 200. 

4) Run the plotResults.py matplotlib script to generate the graph. 

$python plotResults.py
Invalid usage
Usage : python plotResults <fbf_csv> <nonFbf_csv> <expected_counter> <rbf_counter>


-- Fig. 6.8

1) Set up a 4 node cluster as described in the thesis in a star topology

2) Run the following python script to automate the client operations:

$python latencyTestAutomation.py
Invalid Usage!!!!
python <program_name> <coord_ip> <coord_port> <counter_fbf_name> <counter_nofbf_name> <num_of_updates>