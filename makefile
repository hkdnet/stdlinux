all: bin/cat bin/wc_l bin/cat_tab bin/head bin/head_bug bin/tail bin/wc_l_n bin/cat_f

bin/cat: cat.c
	gcc -Wall -o bin/cat cat.c
bin/cat_f: cat_f.c
	gcc -Wall -o bin/cat_f cat_f.c
bin/cat_tab: cat_tab.c
	gcc -Wall -o bin/cat_tab cat_tab.c
bin/head: head.c
	gcc -Wall -o bin/head head.c
bin/head_bug: head_bug.c
	gcc -Wall -o bin/head_bug head_bug.c
bin/tail: tail.c
	gcc -Wall -o bin/tail tail.c
bin/wc_l: wc_l.c
	gcc -Wall -o bin/wc_l wc_l.c
bin/wc_l_n: wc_l_n.c
	gcc -Wall -o bin/wc_l_n wc_l_n.c

.PHONY: all
