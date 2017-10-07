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

SRCS2 = $(wildcard src/model/*.cpp)
OBJS2 = $(SRCS2:.cpp=.o)

all: getdata model

getdata: $(OBJS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

model: $(OBJS2)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) $(LDLIBS) -o $@ $+

clean:
	rm -f -- $(OBJS) $(OBJS2) getdata model

.PHONY: clean all
