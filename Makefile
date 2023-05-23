lexer = src/lexer.l
parser = src/parser.y
gccCompiler = gcc

compiler = run
program = program

# TESTS
testcompiler = tests/RunTests
testsources = $(wildcard tests/*.c)
ext =

ifeq ($(OS), Windows_NT)
	override ext = .exe
endif

ifeq ($(gccCompiler), x86_64-w64-mingw32-gcc)
	override ext = .exe
endif

$(compiler)$(ext): src/lex.yy.c src/parser.tab.c
	$(gccCompiler) src/lex.yy.c src/parser.tab.c -o $(compiler)$(ext)
	
src/parser.tab.c: $(parser)
	bison -d $(parser) -o src/parser.tab.c

src/lex.yy.c: $(lexer)
	flex -osrc/lex.yy.c $(lexer)

check:
ifeq (,$(wildcard $(compiler)$(ext)))
	exit 1
endif


test: $(testsources)
	$(gccCompiler) $(testsources) -o $(testcompiler)

clean:
ifeq ($(OS),Windows_NT)
	del $(compiler)$(ext)
	del src\lex.yy.c
	del src\parser.tab.c
	del src\parser.tab.h
	del output\example_program.ll
	del $(program)$(ext)
else
	rm -f $(compiler)$(ext)
	rm -f src/lex.yy.c
	rm -f src/parser.tab.c
	rm -f src/parser.tab.h
	rm -f output/example_program.ll
	rm -f $(program)$(ext)
endif

compiletest:
	./$(compiler)$(ext) examples/example0.m

runtest:
	clang output/example_program.ll -o $(program)$(ext)

cleantest:
ifeq ($(OS), Windows_NT)
	del output\example_program.ll
	del testprogram
else
	rm -f output/example_program.ll
	rm -f testprogram
endif