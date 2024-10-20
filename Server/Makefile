compiler = g++
flags	 = -Wall
headers	 = graph.hpp mst.hpp protocol.hpp
objects	 = ../MST/graph.o   ../MST/mst.o
main 	 = server
test     = test/client test/dos




all: $(main) $(test)

$(main): %: %.o $(objects)
	$(compiler) $(flags) $^ -o $@

%.o: %.cpp $(headers)
	$(compiler) $(flags) -c $<


clean:
	rm -f *.o $(main) $(test)