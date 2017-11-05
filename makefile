all: bin/cat bin/wc_l bin/cat_tab bin/wc_l_n bin/cat_f

bin/cat: cat.c
	gcc -Wall -o bin/cat cat.c
bin/cat_f: cat_f.c
	gcc -Wall -o bin/cat_f cat_f.c
bin/cat_tab: cat_tab.c
	gcc -Wall -o bin/cat_tab cat_tab.c
bin/wc_l: wc_l.c
	gcc -Wall -o bin/wc_l wc_l.c
bin/wc_l_n: wc_l_n.c
	gcc -Wall -o bin/wc_l_n wc_l_n.c

.PHONY: all
