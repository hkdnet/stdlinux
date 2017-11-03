all: bin/cat bin/wc_l

bin/cat: cat.c
	gcc -Wall -o bin/cat cat.c
bin/wc_l: wc_l.c
	gcc -Wall -o bin/wc_l wc_l.c

.PHONY: all
