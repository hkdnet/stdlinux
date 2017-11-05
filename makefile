all: bin/cat bin/wc_l bin/cat_tab

bin/cat: cat.c
	gcc -Wall -o bin/cat cat.c
bin/cat_tab: cat_tab.c
	gcc -Wall -o bin/cat_tab cat_tab.c
bin/wc_l: wc_l.c
	gcc -Wall -o bin/wc_l wc_l.c

.PHONY: all
