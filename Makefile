execname = run
lexer = src/lexer.l
parser = src/parser.y
compiler = gcc

ifeq ($(OS), Windows_NT)
	exec = $(execname).exe
else
	exec = $(execname).out
endif

$(exec): src/lex.yy.c src/parser.tab.c
	$(compiler) src/lex.yy.c src/parser.tab.c -o $(exec)
	
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
	$(compiler) $(testsources) -o $(testexec)

clean:
ifeq ($(OS),Windows_NT)
	del $(exec)
	del src\lex.yy.c
	del src\parser.tab.c
	del src\parser.tab.h
else
	rm -f $(exec)
	rm -f src/lex.yy.c
	rm -f src/parser.tab.c
	rm -f src/parser.tab.h
endif

runtest:
	make clean
	make
	./run.out examples/tests/test10.m