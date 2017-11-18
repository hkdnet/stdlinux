all: \
	bin/cat bin/cat_tab bin/cat_f \
	bin/grep bin/slice \
	bin/head bin/head_bug bin/tail \
	bin/ls \
	bin/wc_l bin/wc_l_n

bin/cat: cat.c
	gcc -Wall -o bin/cat cat.c
bin/cat_f: cat_f.c
	gcc -Wall -o bin/cat_f cat_f.c
bin/cat_tab: cat_tab.c
	gcc -Wall -o bin/cat_tab cat_tab.c
bin/grep: grep.c
	gcc -Wall -o bin/grep grep.c
bin/head: head.c
	gcc -Wall -o bin/head head.c
bin/head_bug: head_bug.c
	gcc -Wall -o bin/head_bug head_bug.c
bin/ls: ls.c
	gcc -Wall -o bin/ls ls.c
bin/slice: slice.c
	gcc -Wall -o bin/slice slice.c
bin/tail: tail.c
	gcc -Wall -o bin/tail tail.c
bin/wc_l: wc_l.c
	gcc -Wall -o bin/wc_l wc_l.c
bin/wc_l_n: wc_l_n.c
	gcc -Wall -o bin/wc_l_n wc_l_n.c

.PHONY: all
