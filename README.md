# P4-EmbeddedSystems
#### Simple programming language for embedded systems focused on optimizations for memory and power consumption efficiency.
![make](https://github.com/MartinCornelius/P4-EmbeddedSystems/actions/workflows/makefile.yml/badge.svg)

## Getting Started

### Arch
```
sudo pacman -S flex bison make clang llvm
```
### Ubuntu
```
sudo apt-get install flex bison make clang llvm
```
### Windows
Download & Install Chocolatey (https://chocolatey.org/install)

Open a command promot and run the following
```
choco install mingw winflexbison3 llvm
```
Procecced to ``C:\ProgramData\chocolatey\bin`` \
Rename ``mingw32-make.exe`` to ``make.exe`` \
Rename ``win_bison.exe`` to ``bison.exe`` \
Rename ``win_flex.exe`` to ``flex.exe``

Open a **new** command promt, verify using the following that GCC, Make, Bison & Flex is installed correctly
```
gcc --version
make --version
flex --version
bison --version
clang --version
```

If Clang doesn't work properly install MSVC v142 in Visual Studio (Via C/C++)

## Commands
Commands section will be expanded on futher in the future
| Command       | Description  |
| ------------- |------------- |
| make          | Compiles the compiler (Using Flex & Bison) |
| make clean    | Deletes all generated files                |


### Manually compiling and running
Path's are Linux specific
| Command       | Description  |
| ------------- |------------- |
| bison -d src/parser.y -o src/parser.tab.c      | Generates the Parser                         |
| flex -osrc/lex.yy.c src/lexer.l                | Generates the Lexer                          |
| gcc src/lex.yy.c src/parser.tab.c -o run.out   | Compile the compiler                         |
| ./run.out examples/tests/test10.m              | Compile the test10.m and create LLVM code    |
| clang output/example_program.ll -o program.out | Compiles the LLVM code to machine code       |
| ./program.out                                  | Runs the compiled program                    |

## Tools used
* Flex
* Bison
* Make
* GCC
* Github Actions
* Valgrind & Massif visualizer
* LLVM

## Memory usage
```bash
valgrind --tool=massif ./run.out [input file]
```
```bash
massif-visualizer [generated file]
```

## About
#### By Martin, Thomas, Lucas, Lucas, Nicklas and Hugin
