CPPFLAGS=-I../mpl/src -I../brigand/include -I../hana/include
CXXFLAGS=-std=c++14 -Werror -Wall -Wpedantic -ggdb
LDFLAGS=-ggdb
CXX=cxx() { clang++ "$$@" && g++ "$$@"; }; cxx

all: Test
	./$^
clean:
	-rm Test *.o
