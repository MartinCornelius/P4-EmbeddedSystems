import subprocess
import pytest
import shutil

tmpfile = './tests/tmp.m';

# uintTypes = ["uint8", "uint16", "uint32"]
intTypes = ["int8", "int16", "int32"]
floatTypes = ["float16", "float32", "float64"]
operations = ["+", "-", "/", "*"]

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





@pytest.mark.parametrize("intType, operation" , returnArray(intTypes, operations))
def test_arithmeticIntOpertaions(intType, operation):
    expr = "2 " + operation + " 3"
    # expr is evaluted whre the result is conerted to the integer type to prevent floats
    # finally it's converted to a string to be compared with the output
    arithmetiTest(intType, expr, str(int(eval(expr))))