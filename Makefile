exec = Mucas.exe
lexer = src/lexer.l
parser = src/parser.y

$(exec): src/lex.yy.c src/parser.tab.c
	gcc src/lex.yy.c src/parser.tab.c -o $(exec)

src/parser.tab.c: $(parser)
	bison -d $(parser) -o src/parser.tab.c

src/lex.yy.c: $(lexer)
	flex -osrc/lex.yy.c $(lexer)

