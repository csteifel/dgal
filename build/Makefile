VPATH=../src
objects = messaging.o population.o individual.o nodemanager.o

all: $(objects)
	ar cq libdgal.a $(objects)
	@echo "Library built"


messaging.o: messaging.cpp messaging.h
population.o: population.cpp population.h
individual.o: individual.cpp individual.h
nodemanager.o: nodemanager.cpp nodemanager.h


clean:
	-rm -f *.o