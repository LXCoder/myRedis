
myredis-server: main.o MyRedisServer.o MyRedisDb.o CommandTable.o ZSet.o SkipList.o SkipListNode.o
 
	g++ -std=c++11 -o myredis-server main.o MyRedisServer.o MyRedisDb.o CommandTable.o ZSet.o SkipList.o SkipListNode.o 
	  
main.o: main.cpp
	 
	g++ -std=c++11 -c main.cpp

MyRedisServer.o: MyRedisServer.cpp

	g++ -std=c++11 -c MyRedisServer.cpp

MyRedisDb.o: MyRedisDb.cpp

	g++ -std=c++11 -c MyRedisDb.cpp

CommandTable.o: CommandTable.cpp

	g++ -std=c++11 -c CommandTable.cpp

ZSet.o: ZSet.cpp

	g++ -std=c++11 -c ZSet.cpp 

SkipList.o: SkipList.cpp

	g++ -std=c++11 -c SkipList.cpp

SkipListNode.o: SkipListNode.cpp

	g++ -std=c++11 -c SkipListNode.cpp

myredis-cli: myredis_cli.cpp
	g++ -std=c++11 myredis_cli.cpp -o myredis-cli

clean:
	 
	rm -rf *.o
