compilerName = run
programName = program
lexer = src/lexer.l
parser = src/parser.y
gccCompiler = gcc

ifeq ($(OS), Windows_NT)
	ext = .exe
else
	ext = .out
endif

compiler = $(compilerName)$(ext)
program = $(programName)$(ext)

$(compiler): src/lex.yy.c src/parser.tab.c
	$(gccCompiler) src/lex.yy.c src/parser.tab.c -o $(compiler)
	
src/parser.tab.c: $(parser)
	bison -d $(parser) -o src/parser.tab.c

src/lex.yy.c: $(lexer)
	flex -osrc/lex.yy.c $(lexer)

check:
ifeq (,$(wildcard $(compiler)))
	exit 1
endif

# TESTS
testcompiler = tests/RunTests
testsources = $(wildcard tests/*.c)

test: $(testsources)
	$(gccCompiler) $(testsources) -o $(testcompiler)

clean:
ifeq ($(OS),Windows_NT)
	del $(compiler)
	del src\lex.yy.c
	del src\parser.tab.c
	del src\parser.tab.h
	del output\example_program.ll
	del $(program)
else
	rm -f $(compiler)
	rm -f src/lex.yy.c
	rm -f src/parser.tab.c
	rm -f src/parser.tab.h
	rm -f output/example_program.ll
	rm -f $(program)
endif

compiletest:
	./$(compiler) examples/example0.m

runtest:
	clang output/example_program.ll -o $(program)

run:
	make clean
	make
	make compiletest
	make runtest

munit: ./src/lib/munit.h ./src/lib/munit.c ./src/testing.c
	$(CC) -o $@ ./src/lib/munit.c ./src/testing.c
