make:
	bison --yac --defines --output=parser.c parser.y
	flex --outfile=scanner.c al.l
	gcc -o alpha scanner.c parser.c symtab/* quads/*
clean:
	rm *.c *.h alpha