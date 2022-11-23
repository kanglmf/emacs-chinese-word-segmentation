ifeq ($(shell $(CXX) --version | grep -o clang), clang)
	ifneq ($(shell uname -o), Android)
		CXXFLAGS += -std=c++17
	endif
endif

all: cnws
cnws: cnws.cpp
	$(CXX) $(CXXFLAGS) -Icppjieba/deps/limonp/include -Icppjieba/include -O3 -o cnws cnws.cpp
clean:
	rm -f *.a *.o cnws
