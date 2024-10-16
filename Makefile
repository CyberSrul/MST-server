compiler 	 = g++
flags	 	 = -Wall
headers	 	 = graph.hpp mst.hpp protocol.hpp
dependent 	 = tests server
independent  = client dos
main 	 	 = tests
mainout  	 = RunTimes.txt





all: $(dependent) $(independent)

$(dependent): %: %.o graph.o mst.o
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
	gprof -p $(main) gmon.out >> $(mainout);


memcheck: $(main)
	valgrind ./$(main)

callGraph: $(main)
	valgrind --tool=callgrind ./$(main)




clean:
	rm -f *.o *.gcda *.gcno *.gcov *.out callgrind.out.* $(dependent) $(independent) $(mainout)