voronoi1: voronoi1.o dcel.o tower.o read.o
	gcc -Wall -o voronoi1 voronoi1.o dcel.o tower.o read.o -g

voronoi1.o: voronoi1.c dcel.h tower.h read.h
	gcc -Wall -o voronoi1.o voronoi1.c -c -g

dcel.o: dcel.c dcel.h
	gcc -Wall -o dcel.o dcel.c -c -g

tower.o: tower.c tower.h
	gcc -Wall -o tower.o tower.c -c -g

read.o: read.c read.h tower.h
	gcc -Wall -o read.o read.c -c -g

clean: voronoi1
	rm *.o voronoi1