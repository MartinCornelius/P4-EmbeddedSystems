import subprocess
import pytest
import shutil
import random

tmpfile = './tests/tmp.m';

# uintTypes = ["uint8", "uint16", "uint32"]
# intTypes = ["int8", "int16", "int32"]
floatTypes = ["float16", "float32", "float64"]


class IType():
    def __init__(self, type, min, mid, max):
        self.type = type
        self.min = min
        self.mid = mid
        self.max = max
        
        
intTypes = [
    IType("int8", -128, 0, 127), 
    IType("int16", -32768, 0, 32767),
    IType("int32", -2147483648, 0, 2147483647)
]

class Oper():
    def __init__(self, opertaion, reverse):
        self.opertaion = opertaion
        self.reverse = reverse
        
operations = [
    Oper("+", "-"), 
    Oper("-", "+"), 
    Oper("/", "*"),
    Oper("*", "/")
]


def evaluate(min, max, op, val):
    return [
        str(min + op.reverse + val),
        str(0 + op.reverse + val),
        str(max + op.reverse + val)
    ]

def returnArray(typ, op):
    for x in typ:
        for y in op:
            yield x, y

def fileCopyAndOpen(testfile):
    shutil.copyfile(testfile, tmpfile)

    with open(testfile, 'r') as file :
        return file.read()

def fileReplacement(filedata, old, new):
    # Replace the target string
    return filedata.replace(old, new)

def writeToTmp(filedata):
    with open(tmpfile, 'w') as file:
        file.write(filedata)

def arithmetiTest(exprType, expr, expect):
    try:
        filedata = fileCopyAndOpen('./tests/testfiles/arithmetic.m');
        filedata = fileReplacement(filedata, 'exprType', exprType);
        filedata = fileReplacement(filedata, 'expr', expr);
        writeToTmp(filedata);

        subprocess.run(["./run", tmpfile], check=True)
        result = subprocess.run(["lli", "./output/example_program.ll"], stdout=subprocess.PIPE, check=True)
    except Exception as e:
        print(e)
    else:
        assert expect == result.stdout.decode("utf-8").replace("\n", "")

# @pytest.mark.parametrize("intType, op" , returnArray(intTypes, operations))
# def test_arithmeticIntOpertaions(intType, op):
#     rInt = random.randint(intType.mid, intType.max)
    
#     expr = "1+1"
#     # expr = evaluate(intType.min, intType.max, op, rInt);
    
#     # expr is evaluted whre the result is conerted to the integer type to prevent floats
#     # finally it's converted to a string to be compared with the output
#     arithmetiTest(intType.type, expr, str(int(eval(expr))))
#     # arithmetiTest(intType.type, expr[0], str(int(eval(expr[0]))))
#     # arithmetiTest(intType.type, expr[1], str(int(eval(expr[1]))))
#     # arithmetiTest(intType.type, expr[2], str(int(eval(expr[2]))))


scopeTests = [
    
]

def runTest(expect, testFile):
    try:
        subprocess.run(["./run", testFile], check=True)
        result = subprocess.run(["lli", "./output/example_program.ll"], stdout=subprocess.PIPE, check=True)
    except Exception as e:
        print(e)
    else:
        assert str(expect) == result.stdout.decode("utf-8").replace("\n", "")

# Scope tests
def test_GlobalScope():
    runTest(100, "./tests/testfiles/scope/global.m")
    
def test_GlobalScopeRedefine():
    runTest(100, "./tests/testfiles/scope/global_global_redefine.m")
    
def test_LocalScope():
    runTest(100, "./tests/testfiles/scope/local.m")
    
def test_LocalScopeRedefine():
    runTest(100, "./tests/testfiles/scope/local_redefine.m")
    
# Operations
def test_While():
    runTest(100, "./tests/testfiles/operations/while.m")
    
def test_DoubleWhile():
    runTest(1, "./tests/testfiles/operations/doublewhile.m")

pytest.main()