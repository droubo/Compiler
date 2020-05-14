# 
# CS-340 Compilers
# Spring 2020
# 
# Project Phase 3: Intermediate code Generation
# 
# Antonis Ntroumpogiannis    csd4014@csd.uoc.gr
# Georgios Marios Kokiadis csd3962@csd.uoc.gr
# Nikolaos Gounakis        csd3932@csd.uoc.gr
# 
# Makefile
# Compile the alpha parser with this makefile
# 

make:
	bison --yac --defines --output=parser.c parser.y
	flex --outfile=scanner.c al.l
	gcc -o alpha scanner.c parser.c symtab/* quads/* booleanList/* statement/* offset/* -lm -g
	rm *.c *.h
clean:
	rm *.c *.h alpha