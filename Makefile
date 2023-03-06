exec = Mucas.exe
lexer = src/grammar/lexer.l

$(exec): src/grammar/lex.yy.c
	gcc src/grammar/lex.yy.c -o $(exec)

src/grammar/lex.yy.c: $(lexer)
	flex $(lexer)
