CPPFLAGS=-I../mpl/src -I../brigand/include
CXXFLAGS=-std=c++1z -Werror -Wall -Wpedantic -ggdb
LDFLAGS=-ggdb
CXX=cxx() { clang++ "$$@" && g++ "$$@"; }; cxx

all: Test
	./$^
clean:
	-rm Test *.o
