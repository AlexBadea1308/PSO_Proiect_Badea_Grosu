# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -g -std=c++17

# Source files
CLIENT_SRC = sandboxClient.cpp ClientConnection.cpp
SERVER_SRC = servertest.cpp ClientConnection.cpp Server.cpp Database.cpp Table.cpp Column.cpp

# Object files (compiled in the main directory)
CLIENT_OBJS = $(CLIENT_SRC:.cpp=.o)
SERVER_OBJS = $(SERVER_SRC:.cpp=.o)

# Executables
CLIENT_EXE = sandboxClient
SERVER_EXE = servertest

# Default target
all: $(CLIENT_EXE) $(SERVER_EXE)

# Rule to build client executable
$(CLIENT_EXE): $(CLIENT_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(CLIENT_OBJS)

# Rule to build server executable
$(SERVER_EXE): $(SERVER_OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $(SERVER_OBJS)

# Compile .cpp files into .o object files in the main directory
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean up the build (removes object files and executables)
clean:
	rm -f $(CLIENT_OBJS) $(SERVER_OBJS) $(CLIENT_EXE) $(SERVER_EXE)

# Phony targets (not actual files)
.PHONY: all clean
