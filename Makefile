compiler = g++
flags	 = -Wall
headers	 = graph.hpp mst.hpp protocol.hpp
progs 	 = tests client server
main 	 = tests





all: $(progs)

tests: tests.o graph.o mst.o
	$(compiler) $(flags) $^ -o $@

server: server.o graph.o mst.o protocol.o
	$(compiler) $(flags) $^ -o $@

%.o: %.cpp $(headers)
	$(compiler) $(flags) -c $<




coverage: flags += -fprofile-arcs -ftest-coverage -fprofile-filter-files=".*\.cpp"
coverage: $(main)
	./$(main)
	gcov *.cpp


profile: flags += -pg
profile: $(main)
	./$(main)
	gprof -p $(main) gmon.out >> RunTimes.txt;


memcheck: $(main)
	valgrind ./$(main)

callGraph: $(main)
	valgrind --tool=callgrind ./$(main)




clean:
	rm -f *.o *.gcda *.gcno *.gcov *.txt *.out callgrind.out.* $(progs)