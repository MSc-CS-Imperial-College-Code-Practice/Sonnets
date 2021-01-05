exec: main.o sonnet.o
	  	g++ -Wall -g main.o sonnet.o -o exec

main.o: main.cpp sonnet.h
		g++ -Wall -g -c main.cpp

sonnet.o: sonnet.cpp sonnet.h
		  g++ -Wall -g -c sonnet.cpp

clean:
	  rm -f *.o exec