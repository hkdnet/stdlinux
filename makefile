all: \
	bin/cat bin/cat_tab bin/cat_f \
	bin/grep bin/slice \
	bin/head bin/head_bug bin/tail \
	bin/ls \
	bin/daytime \
	bin/mkdir \
	bin/shell \
	bin/sigwait \
	bin/wc_l bin/wc_l_n \
	bin/httpserver

bin/cat: cat.c
	gcc -Wall -o $@ $^
bin/cat_f: cat_f.c
	gcc -Wall -o $@ $^
bin/cat_tab: cat_tab.c
	gcc -Wall -o $@ $^
bin/daytime: daytime.c
	gcc -Wall -o $@ $^
bin/grep: grep.c
	gcc -Wall -o $@ $^
bin/head: head.c
	gcc -Wall -o $@ $^
bin/head_bug: head_bug.c
	gcc -Wall -o $@ $^
bin/httpserver: httpserver.c
	gcc -Wall -o $@ $^
bin/ls: ls.c
	gcc -Wall -o $@ $^
bin/mkdir: mkdir.c
	gcc -Wall -o $@ $^
bin/slice: slice.c
	gcc -Wall -o $@ $^
bin/shell: shell.c
	gcc -Wall -o $@ $^
bin/sigwait: sigwait.c
	gcc -Wall -o $@ $^
bin/tail: tail.c
	gcc -Wall -o $@ $^
bin/wc_l: wc_l.c
	gcc -Wall -o $@ $^
bin/wc_l_n: wc_l_n.c
	gcc -Wall -o $@ $^
test: all
	test/test_tail
	test/test_httpserver

.PHONY: all
