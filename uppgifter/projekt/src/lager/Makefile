tree.o: tree.c
	gcc -Wall -std=c11 -ggdb -c tree.c

%.o: %.c
	gcc -Wall -std=c11 -pedantic -ggdb -c $<

db: db.o tui.o tree.o list.o iterator.o undo.o
	gcc -Wall -std=c11 -pedantic -ggdb -o db db.o tui.o tree.o list.o iterator.o undo.o

gc_perf_test:
	gcc -Wall -std=c11 -pedantic -O1 gc_perf_test.c list.c iterator.c -o gc_perf_test

tree_test: tree.o tree_test.o
	gcc -Wall -std=c11 -o tree_test tree_test.o tree.o

test: tree_test db
	#valgrind --leak-check=full ./db
	cat ./start.txt - | valgrind --leak-check=full ./db

run_perf: gc_perf_test
	@echo "Running 6 times, discount best and worst case"
	@echo "------ 1 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 2 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 3 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 4 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 5 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null
	@echo "------ 6 ---------------------"
	time ./gc_perf_test 100000 10000 > /dev/null

clean:
	rm -f *.o gc_perf_test

