# Makefile
#
# Makefile for the C++ code.

CXX=c++
CXXFLAGS= -Wall -Wextra -std=c++0x
CXXSRCS1=src/client.cpp src/socket.cpp src/message.cpp
CXXSRCS2=src/server.cpp src/socket.cpp src/message.cpp src/termination.cpp
CXXSRCS3=src/server_fail.cpp src/socket.cpp src/message.cpp src/termination.cpp
CXXOBJS1=$(CXXSRCS1:.cpp=.o)
CXXOBJS2=$(CXXSRCS2:.cpp=.o)
CXXOBJS3=$(CXXSRCS3:.cpp=.o)
LIBS=
INCLUDES=-I./include
TARGET1=client
TARGET2=server
TARGET3=server_fail
#DEBUGFLAGS=-g -ggdb -O0 -fno-inline

# Required for the target clean.
RM=rm -rf

CXXFLAGS+=$(DEBUGFLAGS) $(INCLUDES) $(LIBS)

all : $(CXXSRCS1) $(CXXOBJS1) $(TARGET1) $(CXXSRCS2) $(CXXOBJS2) $(TARGET2)\
	$(CXXSRCS3) $(CXXOBJS3) $(TARGET3)

$(TARGET1) : $(CXXOBJS1)
	$(CXX) $(CXXOBJS1) -o $(TARGET1)

$(TARGET2) : $(CXXOBJS2)
	$(CXX) $(CXXOBJS2) -o $(TARGET2)

$(TARGET3) : $(CXXOBJS3)
	$(CXX) $(CXXOBJS3) -o $(TARGET3)

.cpp.o :
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean :
	$(RM) src/*.o src/*.dSYM $(TARGET1) $(TARGET2) $(TARGET3)

depend : .depend

.depend : $(CXXSRCS1) $(CXXSRCS2) $(CXXSRCS3)
	rm -f ./.depend
	$(CXX) $(CXXFLAGS) -MM $^ -MF ./.depend

.PHONY : clean

include .depend
