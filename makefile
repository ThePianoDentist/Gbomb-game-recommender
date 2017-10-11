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

SRCS = $(wildcard src/getdata/*.cpp)
OBJS = $(SRCS:.cpp=.o)

SRCS_MODEL = $(wildcard src/model/*.cpp)
OBJS_MODEL = $(SRCS_MODEL:.cpp=.o)

SRCS_USE = $(wildcard src/use/*.cpp)
OBJS_USE = $(SRCS_USE:.cpp=.o)

all: getdata build use

getdata: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

build: $(OBJS_MODEL)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

use: $(OBJS_USE) src/model/model.cpp
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

clean:
	rm -f -- $(OBJS) $(OBJS_MODEL) $(OBJS_USE) getdata model use

.PHONY: clean all
