pack:main.o
	g++ -o pack main.o
main.o:main.cpp
	g++ -c main.cpp
clean:
	rm *~ *.o


grec:generate_rec.o
	g++ -o grec generate_rec.o
generate_rec.o:generate_rec.cpp
	g++ -c generate_rec.cpp

