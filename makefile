pack:main.o
	g++ -o pack main.o
main.o:main.cpp
	g++ -c main.cpp
clean:
	rm *~ *.o
