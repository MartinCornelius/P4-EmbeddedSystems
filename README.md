# P4-EmbeddedSystems
#### Simple programming language for embedded systems focused on optimizations for memory and power consumption efficiency.
![make](https://github.com/MartinCornelius/P4-EmbeddedSystems/actions/workflows/makefile.yml/badge.svg)

## Setup
```
make
./run [input file path]
```
## Commands
```
make
```
```
make test
```
```
make clean
```

## Tools used
* Flex
* Bison
* Make
* GCC
* Github Actions
* Valgrind & Massif visualizer

## Memory usage
```bash
valgrind --tool=massif ./run.out [input file]
```
```bash
massif-visualizer [generated file]
```

## About
#### By Martin, Thomas, Lucas, Lucas, Nicklas and Hugin
