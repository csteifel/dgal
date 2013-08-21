VPATH=../src
objects = messenger.o population.o individual.o nodemanager.o

all: $(objects)
	ar cq libdgal.a $(objects)
	@echo "Library built"


messenger.o: messenger.cpp messenger.h
population.o: population.cpp population.h
individual.o: individual.cpp individual.h
nodemanager.o: nodemanager.cpp nodemanager.h


clean:
	-rm -f *.o