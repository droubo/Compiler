samuraivsninja:
	rm -rf *.out
killtxt:
	rm -rf *.txt
all:
	flex --outfile=lex.c al.l
	gcc -o ninja.out lex.c
	gcc -o samurai.out bonus.c
