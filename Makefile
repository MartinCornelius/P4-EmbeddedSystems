exec = Mucas.exe
lexer = src/lexer.l

$(exec): src/lex.yy.c
	gcc src/lex.yy.c -o $(exec)

src/lex.yy.c: $(lexer)
	flex -osrc/lex.yy.c $(lexer)

