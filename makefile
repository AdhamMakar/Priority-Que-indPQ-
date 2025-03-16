run: Test
	-./Test

all: Test

Test: Test.o NextPrime.o
	g++ -o Test Test.o NextPrime.o

Test.o: Test.cpp Heap.h
	g++ -c Test.cpp

NextPrime.o: NextPrime.cpp Heap.h
	g++ -c NextPrime.cpp

clean:
	rm -f *.o Test NextPrime
