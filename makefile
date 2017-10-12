CXX = g++
CXXFLAGS = -Wall -Werror -Wno-int-in-bool-context -std=c++14 -ggdb -Og
# -Wno-int-in-bool-context due to eigen throwing it a lot

# Librarys
PKGS = eigen3

INCLUDE := $(foreach pkg,$(PKGS),$(shell pkg-config --cflags $(pkg)))
CXXFLAGS += $(INCLUDE)
LDLIBS := $(foreach pkg,$(PKGS),$(shell pkg-config --libs $(pkg)))

LDFLAGS = -L/usr/local/lib
LDLIBS += -lboost_system -lcrypto -lssl -lcpprest -lcppdb -lboost_serialization 

SRCS = $(wildcard src/core/*.cpp)
OBJS = $(SRCS:.cpp=.o)

SRCS_GD = $(wildcard src/getdata/*.cpp)
OBJS_GD = $(SRCS_GD:.cpp=.o)

SRCS_TRAIN = $(wildcard src/train/*.cpp)
OBJS_TRAIN = $(SRCS_TRAIN:.cpp=.o)

SRCS_USE = $(wildcard src/use/*.cpp)
OBJS_USE = $(SRCS_USE:.cpp=.o)

all: getdata train use

getdata: $(OBJS_GD) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

train: $(OBJS_TRAIN) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

use: $(OBJS_USE) $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

clean:
	rm -f -- $(OBJS) $(OBJS_TRAIN) $(OBJS_USE) $(OBJS_GD) getdata train use

.PHONY: clean all
