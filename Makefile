discord.exe : request.o main.o  parser.o
		g++ -o discord.exe request.o parser.o main.o -lcurl 
		
				
request.o : api/request.cpp 
		g++ -c api/request.cpp 

parser.o : api/parser.cpp
		g++ -c api/parser.cpp -Ithird-party/rapidjson/include

main.o : main.cpp 
		g++ -c main.cpp 

clean :
		rm discord.exe request.o parser.o main.o