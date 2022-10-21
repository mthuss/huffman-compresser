main:
	gcc funcoes.c compact.c -o compact -lm

clang:
	clang funcoes.c compact.c -o compact -lm
