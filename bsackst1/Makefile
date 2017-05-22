
FLAGS = -g -Wall -pedantic -Wextra  -std=c++11

all: clean project2

project2: project2.o 
	g++ $(FLAGS) project2.o -o cache-sim

project2.o: project2.cpp
	g++ $(FLAGS) -c project2.cpp -o project2.o

clean:
	rm -f *.o cache-sim