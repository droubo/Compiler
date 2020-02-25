# 
# CS-240 Compilers
# Spring 2020
# 
# Project Phase 1: Lexical Analysis
# 
# Antonis Droubogiannis    csd4014@csd.uoc.gr
# Georgios Marios Kokiadis csd3962@csd.uoc.gr
# Nikolaos Gounakis        csd3932@csd.uoc.gr
#
# Makefile for Lexical Analyzer
# 

CC = gcc
CFLAGS =
DEPS = 
SUCC_COLOR=\033[32;01m
WARN_COLOR=\033[0;33m
ERROR_COLOR=\033[31;01m
NO_COLOR=\033[0m

%.c: %.l
	@echo "$(SUCC_COLOR)>>$(WARN_COLOR) Converting .l to .c... $(NO_COLOR)"
	flex --outfile=al.c $^

%.o : %.c %.h
	@echo "$(SUCC_COLOR)>>$(WARN_COLOR) Compiling Object files... $(NO_COLOR)"
	$(CC) $^ -c $(CFLAGS)

make: al.c
	@echo "$(SUCC_COLOR)>>$(WARN_COLOR) Compiling implementation... $(NO_COLOR)"
	@if $(CC) $^ -o al $(CFLAGS) ; then echo "$(SUCC_COLOR)Compilation Complete!$(NO_COLOR)"; fi;

.PHONY: clean

clean:
	@echo "$(SUCC_COLOR)Cleaning up...$(NO_COLOR)"
	@if rm *.o; then echo "$(SUCC_COLOR)Clean up of Object files done!$(NO_COLOR)"; else echo "$(ERROR_COLOR)No Object files to remove!$(NO_COLOR)"; fi;
	@if rm *.h.gch; then echo "$(SUCC_COLOR)Clean up of Precompiled Libs done!$(NO_COLOR)"; else echo "$(ERROR_COLOR)No Precompiled Libs to remove!$(NO_COLOR)"; fi;
	@echo  "$(SUCC_COLOR)Clean up Done!$(NO_COLOR)"