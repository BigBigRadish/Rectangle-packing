pack:rec_packing.o
	g++ -o pack rec_packing.o
rec_packing.o:rec_packing.cpp
	g++ -c rec_packing.cpp
clean:
	rm *~ *.o
