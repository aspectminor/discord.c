CFLAGS=-Ithird-party/rapidjson/include
CXXFLAGS=

discord.exe : request.o main.o  parser.o
		g++ -o discord.exe request.o parser.o main.o -lcurl 
		
				
request.o : api/request.cpp 
		g++ -c api/request.cpp $(CXXFLAGS)

parser.o : api/parser.cpp
		g++ -c api/parser.cpp $(CFLAGS)

main.o : main.cpp 
		g++ -c main.cpp $(CFLAGS)

clean :
		rm discord.exe request.o parser.o main.o