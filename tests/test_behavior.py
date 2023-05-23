import subprocess
import pytest
import shutil
import random
import platform
from pathlib import Path

if (platform.system() == "Windows"):
    print("ay")

tmpfile = 'tests/tmp.m';

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

def runTest(expect, testFile):
    # Delete test files to ensure they are not reused
    subprocess.run(["make", "cleantest"])
    subprocess.run(["make"])
    
    # # Compile our test file
    if (platform.system() == "Windows"):
        # Compile our test file
        subprocess.run(["run.exe", testFile])
         # Compile llvm code
        subprocess.run(["clang",  "output/example_program.ll", "-otestprogram.exe"])
        # Run the compiled program
        result = subprocess.run(["testprogram.exe"], stdout=subprocess.PIPE)
    else:
        # Compile our test file
        subprocess.run(["./run", "./" + testFile])
            # Compile llvm code
        subprocess.run(["clang",  "./output/example_program.ll", "-otestprogram"])
        # Run the compiled program
        result = subprocess.run(["./testprogram"], stdout=subprocess.PIPE)

    assert str(expect) == result.stdout.decode("utf-8").replace("\n", "")

@pytest.mark.parametrize(
    "type, test_input, expected",
    [
        #### Integer ####
        ("int8", "100+27", "127"),
        ("int8", "0 - 127", "-127"),
        ("int8", "100*0", "0"),
        ("int8", "100/5", "20"),
        
        ("int16", "32700+67", "32767"),
        ("int16", "0 - 32767", "-32767"),
        ("int16", "100*0", "0"),
        ("int16", "100/5", "20"),
        
        ("int32", "2147483600+47", "2147483647"),
        ("int32", "0 - 2147483647", "-2147483647"),
        ("int32", "100*0", "0"),
        ("int32", "100/5", "20"),
        
        #### Unsigned ####
        ("uint8", "200+55", "255"),
        ("uint8", "100*0", "0"),
        ("uint8", "100/5", "20"),
        
        ("uint16", "65500+35", "65535"),
        ("uint16", "100*0", "0"),
        ("uint16", "100/5", "20"),
        
        ("uint32", "2147483600+47", "2147483647"),
        ("uint32", "100*0", "0"),
        ("uint32", "100/5", "20"),
        
        #### Float ####
        # ("float16", "200.00+55.00", "255.00"),
        # # ("float16", "-100-27", "-127"),
        # ("float16", "100*0", "0"),
        # ("float16", "100/5", "20"),
        
        # ("float32", "65500+35", "65535"),
        # # ("float32", "-3270-67", "-32767"),
        # ("float32", "100*0", "0"),
        # ("float32", "100/5", "20"),
        
        # ("float64", "4294967200+95", "4294967295"),
        # # ("float64", "-2147483600-47", "-2147483647"),
        # ("float64", "100*0", "0"),
        # ("float64", "100/5", "20"),
    ]
)
def test_arithmeticIntOpertaions(type, test_input, expected):
    filedata = fileCopyAndOpen('tests/testfiles/templates/arithmetic.m');
    filedata = fileReplacement(filedata, 'exprType', type);
    filedata = fileReplacement(filedata, 'expr', test_input);
    writeToTmp(filedata);
    
    if (str(int(eval(test_input))) != expected):
        print("Something went wrong!")
    
    # runTest(str(int(eval(test_input))), tmpfile)
    runTest(expected, tmpfile)
    
@pytest.mark.parametrize(
    "value, expr, expected",
    [
        ("59", "100 > 99", "59"),
        ("43", "100 < 150", "43"),
        ("5", "100 != 50", "5"),
    ]
)
def test_ifStatement(value, expr, expected):
    filedata = fileCopyAndOpen('tests/testfiles/templates/ifstatement.m');
    filedata = fileReplacement(filedata, 'value', value);
    filedata = fileReplacement(filedata, 'expr', expr);
    writeToTmp(filedata);
    
    # runTest(str(int(eval(test_input))), tmpfile)
    runTest(expected, tmpfile)

# Scope tests
def test_LocalScope():
    runTest(100, "tests/testfiles/scope/local.m")
    
# def test_LocalScopeRedefine(): # This one does not work on Windows for some odd reason
#     runTest(100, "tests/testfiles/scope/localredefine.m")
    
# Operations
def test_While():
    runTest(99, "tests/testfiles/operations/while.m")
    
def test_DoubleWhile():
    runTest(11, "tests/testfiles/operations/doublewhile.m")
    
# Function
def test_Function():
    runTest(570, "tests/testfiles/operations/function.m")
    
def test_Functions():
    runTest(5705, "./tests/testfiles/operations/functions.m")
    
# Scope tests
def test_GlobalScope():
    runTest(137, "./tests/testfiles/scope/global.m")
    
# def test_GlobalScopeRedefine():
#     runTest(598, "./tests/testfiles/scope/globalredefine.m")

pytest.main()