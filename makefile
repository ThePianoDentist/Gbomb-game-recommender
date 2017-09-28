CXX = g++
CXXFLAGS = -Wall -Werror -std=c++14

# Librarys
INCLUDE = -I/usr/local/include
LDFLAGS = -L/usr/local/lib
LDLIBS = -lboost_system -lcrypto -lssl -lcpprest -lpqxx -lpq -lcppdb

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)

getdata: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

clean:
	rm -f -- $(OBJS) getdata

.PHONY: clean
