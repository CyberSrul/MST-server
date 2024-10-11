compiler = g++
flags	 = -Wall -g3
headers	 = graph.hpp mst.hpp
objects	 = graph.o	 mst.o   tests.o
main 	 = tests




all: $(main)

$(main): $(objects)
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
	rm -f *.o *.gcda *.gcno *.gcov *.txt *.out callgrind.out.* $(main)