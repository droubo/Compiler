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
# Make the alpha compiler and VM
# 

ascc:
	bison --yac --defines --output=Phase4/parser.c Phase4/parser.y
	flex --outfile=Phase4/scanner.c Phase4/al.l
	gcc -o ascc Phase4/scanner.c Phase4/parser.c Phase4/symtab/* Phase4/quads/* Phase4/booleanList/* Phase4/statement/* Phase4/offset/* Phase4/target_code/* -lm -g
	rm Phase4/*.c Phase4/*.h

avm:
	gcc Phase5/avm/avm_controller.c -lm -o avm

all: ascc avm