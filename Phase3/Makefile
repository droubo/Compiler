make:
	bison --yac --defines --output=parser.c parser.y
	flex --outfile=scanner.c al.l
	gcc -o alpha scanner.c parser.c symtab/* quads/* booleanList/* statement/* offset/* -lm -g
clean:
	rm *.c *.h alpha