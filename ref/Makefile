Simplify: Simp.o Surface.o SimpVertexClustering.o SimpELEN.o SimpQEM.o Classes.h common.o
	g++ -g -pg -O3 -std=c++14 -fopenmp Simp.o common.o Classes.h SimpQEM.o SimpELEN.o  SimpVertexClustering.o Surface.o Vector3f.o -o Simplify

Simp.o: Surface.o Simp.cpp
	g++ -g -O3 -pg -std=c++14 -c Simp.cpp

SimpVertexClustering.o: Surface.o SimpVertexClustering.cpp SimpVertexClustering.h
	g++ -g -O3 -pg -std=c++14 -c SimpVertexClustering.cpp

SimpELEN.o: Surface.o SimpELEN.cpp SimpELEN.h
	g++ -g -O3 -pg -fopenmp -std=c++14 -c SimpELEN.cpp

SimpQEM.o: Surface.o SimpELEN.o SimpQEM.cpp SimpQEM.h
	g++ -g -O3 -pg -fopenmp -std=c++14 -c SimpQEM.cpp

Surface.o: Surface.h Surface.cpp Vector3f.o
	g++ -g -O3 -pg -std=c++14 -c Surface.cpp -lCGAL -frounding-math

Vector3f.o: Vector3f.h Vector3f.cpp
	g++ -g -O3 -pg -std=c++14 -c Vector3f.cpp

common.o: common.h common.cpp
	g++ -g -O3 -pg -std=c++14 -c common.cpp

clean:
	rm *.o Simplify
