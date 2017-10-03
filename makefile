CXX = g++
CXXFLAGS = -Wall -Werror -std=c++14

# Librarys
INCLUDE = -I/usr/local/include/ -I/usr/local/include/eigen3
LDFLAGS = -L/usr/local/lib
LDLIBS = -lboost_system -lcrypto -lssl -lcpprest -lcppdb 

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

SRCS2 = $(wildcard src/run/*.cpp)
OBJS2 = $(SRCS2:.cpp=.o)

all: getdata run

getdata: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

run: $(OBJS2)
	$(CXX) $(CXXFLAGS) $(INCLUDE) $(LDFLAGS) $(LDLIBS) -g -o $@ $+

clean:
	rm -f -- $(OBJS) getdata run

.PHONY: clean
