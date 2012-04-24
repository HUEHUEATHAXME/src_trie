all: compile
	./testtrie

compile:
	gcc -g -o testtrie testtrie.c trie.c plot.c

clean:
	rm -f *.o
	rm -f *~
	rm -f ./testtrie
	rm -f *.ps
	rm -f plot_*
