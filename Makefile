execname = run
lexer = src/lexer.l
parser = src/parser.y

ifeq ($(OS),Windows_NT)
	exec = $(execname).exe
else
	exec = $(execname).out
endif

$(exec): src/lex.yy.c src/parser.tab.c
	gcc src/lex.yy.c src/parser.tab.c -o $(exec)
	
src/parser.tab.c: $(parser)
	bison -d $(parser) -o src/parser.tab.c

src/lex.yy.c: $(lexer)
	flex -osrc/lex.yy.c $(lexer)

check:
ifeq (,$(wildcard $(exec)))
	exit 1
endif

# TESTS
testexec = tests/RunTests
testsources = $(wildcard tests/*.c)

test: $(testsources)
	gcc $(testsources) -o $(testexec)
