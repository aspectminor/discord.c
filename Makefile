CFLAGS=

discord.exe : fetch.o main.o  parser.o
		gcc -o discord.exe fetch.o main.o -lssl

websocket.exe : fetch.o websocket.o json.o
	gcc -o websocket.exe fetch.o websocket.o json.o -lssl -lm
		
				
fetch.o : api/socket/fetch.c 
		gcc -c api/socket/fetch.c $(CFLAGS)

main.o : main.c 
		gcc -c main.c $(CFLAGS)
	
websocket.o : api/socket/websocket.c
		gcc -c api/socket/websocket.c $(CFLAGS) -DDISCORD_TOKEN=\"$(DISCORD_TOKEN)\"

json.o : lib/json.c
		gcc -c lib/json.c $(CFLAGS)

clean :
		rm discord.exe fetch.o main.o