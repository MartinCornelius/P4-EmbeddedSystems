exec = Mucas.exe

$(exec): lex.yy.c
	gcc lex.yy.c -o $(exec)

lex.yy.c: src/grammar/lexer.l
	flex src/grammar/lexer.l
