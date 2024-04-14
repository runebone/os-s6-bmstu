.PHONY: all, com, pm

all: a.out
	@./a.out \
	| sed "s/                 //g"

com: a.out
	@gcc -g -O0 -DNTHREADS=16 -DENV=1 main.c && ./a.out \
	| sh comment_stat.sh \
	| sh comment_environ.sh \
	| sed "s/                 //g"
	# @gcc -O0 -DNTHREADS=1024 -DENV=1 main.c && ./a.out \

pm: pm.c
	@gcc pm.c -o pm
	@make com | sh get_pid_and_maps.sh

a.out: main.c
	@gcc -g -O0 -DNTHREADS=16 -DENV=1 main.c
