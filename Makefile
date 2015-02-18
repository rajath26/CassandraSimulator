CFLAGS = -Wall -g -std=c++11 

all: CassandraNode Client

CassandraNode: Node.o Message.o tcp.o NodeId.o Counter.o Shard.o FBF.o
	g++ -o CassandraNode Node.o Message.o tcp.o NodeId.o Counter.o FBF.o Shard.o ${CFLAGS}
	#g++ -o CassandraNode Node.o Message.o tcp.o NodeId.o Counter.o Shard.o ${CFLAGS}
	
Client: Client.o tcp.o Message.o
	g++ -o Client Client.o tcp.o Message.o ${CFLAGS}

Node.o: Node.cpp Node.h Common.h Message.h tcp.h NodeId.h Counter.h
	g++ -c Node.cpp ${CFLAGS}

Message.o: Message.cpp Message.h Common.h 
	g++ -c Message.cpp ${CFLAGS}

tcp.o: tcp.cpp tcp.h Common.h 
	g++ -c tcp.cpp ${CFLAGS} 
	
NodeId.o: NodeId.cpp NodeId.h Common.h
	g++ -c NodeId.cpp ${CFLAGS}
	
Counter.o: Counter.cpp Counter.h Common.h Shard.h FBF.h
	g++ -c Counter.cpp ${CFLAGS}
	
FBF.o: FBF.cpp FBF.h Common.h bloom_filter.hpp
	g++ -c FBF.cpp ${CFLAGS}
 
Shard.o: Shard.cpp Shard.h Common.h
	g++ -c Shard.cpp ${CFLAGS}
	
Client.o: Client.cpp Client.h tcp.h Common.h Message.h
	g++ -c Client.cpp ${CFLAGS}
	
clean: 
	rm -rf *.o CassandraNode Client
