compiler 	 = g++
flags	 	 = -Wall -O3
headers	 	 = MST/graph.hpp MST/mst.hpp Server/protocol.hpp
objects		 = MST/graph.o	 MST/mst.o
dependent 	 = MST/tests Server/server
independent  = Server/test/client Server/test/dos
main 	 	 = MST/tests
mainout  	 = RunTimes.txt





all: $(dependent) $(independent)

$(dependent): %: %.o $(objects)
	$(compiler) $(flags) $^ -o $@

%.o: %.cpp $(headers)
	$(compiler) $(flags) -c $< -o $@




coverage: flags += -fprofile-arcs -ftest-coverage -fprofile-filter-files=".*\.cpp"
coverage: $(main)
	./$(main)
	gcov MST/*.cpp


profile: flags += -pg
profile: $(main)
	./$(main)
	gprof -p $(main) gmon.out >> $(mainout);


memcheck: $(main)
	valgrind ./$(main)

callGraph: $(main)
	valgrind --tool=callgrind ./$(main)




clean:
	@for dir in . Server/ Server/test MST; do \
    rm -f $$dir/*.o $$dir/*.gcda $$dir/*.gcno $$dir/*.gcov $$dir/*.out $$dir/callgrind.out.* $$dir/$(dependent) $$dir/$(independent) $$dir/$(mainout); \
done