CXX = g++
FILE_PATH = $(shell pwd)/../../
CPPFLAGS = -I$(FILE_PATH) -std=c++17 -pthread -Wall
SERVER_APP = ServerApp
CLIENT_APP = ClientApp
SERVER_CPPFILES = main_server.cpp
CLIENT_CPPFILES = main_client.cpp
TARGETS = $(SERVER_APP) $(CLIENT_APP)


all: $(TARGETS)


$(SERVER_APP): $(SERVER_CPPFILES)
	$(CXX) $(CPPFLAGS) $(SERVER_CPPFILES) -o $(SERVER_APP)


$(CLIENT_APP): $(CLIENT_CPPFILES)
	$(CXX) $(CPPFLAGS) $(CLIENT_CPPFILES) -o $(CLIENT_APP)
	

memcheck_server:
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./$(SERVER_APP)
	
	
memcheck_client:
	valgrind --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=20 --track-fds=yes ./$(CLIENT_APP)
	

clean:
	rm -f $(TARGETS)
