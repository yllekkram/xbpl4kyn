CXX					= g++
CXXFLAGS		= -Wall
CXXINC			= -Iinclude -Itest/include
CXXLD				= $(CXX)
CXXLDFLAGS	=
RM					= rm

vpath %.hpp include:test/include

EXE			= ElevatorTestServer UDPTestServer testTCP testUDP testElevatorController ElevatorSimulatorTest
XENOEXE			= main
SRCS			= $(wildcard *.cpp)
OBJS			= ${SRCS:.cpp=.opp}

# clear out all the suffixes
.SUFFIXES:
# list only those that we use
.SUFFIXES: .opp .cpp

# define a suffix rule for .cpp -> .opp
.cpp.opp:
	$(CXX) $(CXXINC) -o $@ -c $< $(CXXFLAGS) 

all:: $(EXE)

###### Xenomai Config ######
### Note: to override the search path for the xeno-config script, use "make XENO=..."

###### USER SPACE BUILD (no change required normally) ######
ifeq ($(KERNELRELEASE),)
ifneq ($(XENOEXE),)

### Default Xenomai installation path
XENO ?= /usr/xenomai

XENOCONFIG=$(shell PATH=$(XENO):$(XENO)/bin:$(PATH) which xeno-config 2>/dev/null)

### Sanity check
ifeq ($(XENOCONFIG),)
xeno::
	@echo ">>> Invoke make like this: \"make XENO=/path/to/xeno-config\" <<<"
	@echo
else
xeno::$(XENOEXE)
CXXFLAGS+=$(shell $(XENOCONFIG) --xeno-cflags) $(MY_CFLAGS)

CXXLDFLAGS=$(shell $(XENOCONFIG) --xeno-ldflags) $(MY_LDFLAGS) -lnative

# This includes the library path of given Xenomai into the binary to make live
# easier for beginners if Xenomai's libs are not in any default search path.
CXXLDFLAGS+=-Xlinker -rpath -Xlinker $(shell $(XENOCONFIG) --libdir)
endif
endif
endif
###### End Xenomai Config ######

xeno:: $(XENOEXE)

main: main.opp Heap.opp ElevatorSimulator.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

tests: test/test.opp test/testElevatorController.opp test/testHeap.opp test/testMessage.opp Message.opp Heap.opp ElevatorCommon.opp
	$(CXX) $(CXXINC) -o test/test $^ test/lib/libUnitTest++.a $(CXXFLAGS)

clean:
	-$(RM) -f $(EXE) $(OBJS) $(wildcard *.gch) $(wildcard test/*.opp) test/test

ElevatorSimulatorTest: ElevatorSimulator.opp ElevatorSimulatorTest.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

ElevatorTestServer: ElevatorCommon.opp ElevatorTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

testTCP: ElevatorCommon.opp ElevatorController.opp Message.opp testTCP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
UDPTestServer: ElevatorCommon.opp UDPTestServer.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testUDP: ElevatorCommon.opp ElevatorController.opp Message.opp UDPView.opp testUDP.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^
	
testElevatorController: ElevatorCommon.opp ElevatorController.opp Message.opp UDPView.opp testElevatorController.opp
	$(CXXLD) $(CXXLDFLAGS) -o $@ $^

ElevatorCommon.opp: ElevatorCommon.hpp Exception.hpp
ElevatorController.opp: ElevatorCommon.hpp Exception.hpp Heap.hpp Message.hpp
ElevatorControllerView.opp: ElevatorControllerView.hpp
ElevatorSimulator.opp: ElevatorSimulator.hpp
ElevatorSimulatorTest.opp: ElevatorSimulator.hpp
ElevatorTestServer.opp: ElevatorCommon.hpp
Heap.opp: Heap.hpp
Message.opp: Message.hpp ElevatorCommon.hpp Exception.hpp
UDPTestServer.opp: ElevatorCommon.hpp
UDPView.opp: ElevatorControllerView.hpp Exception.hpp UDPView.hpp
testElevatorController.opp: ElevatorCommon.hpp ElevatorController.hpp Exception.hpp Message.hpp UDPView.hpp
testTCP.opp: ElevatorCommon.hpp
testUDP.opp: ElevatorCommon.hpp ElevatorController.hpp Message.hpp UDPView.hpp

test.opp: UnitTest++.h
testElevatorController.opp: ElevatorController.hpp ElevatorController.cpp
testHeap.opp: Heap.hpp
testMessage.opp: Message.hpp
