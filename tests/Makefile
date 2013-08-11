VPATH=../build:../src
objects = main.o
includes = 
libs = -L../build -ldgal
CXXFLAGS = -Wall -Wextra $(includes) $(libs)

all: $(objects)
	$(CXX) $(CXXFLAGS) -o test.out $(objects)
	@echo "Tests built"

main.o: main.cpp

clean:
	-rm -f *.o