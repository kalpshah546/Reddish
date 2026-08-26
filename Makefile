CXX = g++
CXXFLAGS = -std=c++11 -Wall -Wextra -g -Iinclude -pthread -O2

SERVER_OBJS = build/ReddishCommandHandler.o \
              build/ReddishServer.o \
              build/main.o

CLIENT_OBJS = build/client_test.o

all: my_server client_test

my_server: $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) $(SERVER_OBJS) -o my_server

client_test: $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) $(CLIENT_OBJS) -o client_test

build/%.o: src/%.cpp
	mkdir -p build
	$(CXX) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf build my_server client_test