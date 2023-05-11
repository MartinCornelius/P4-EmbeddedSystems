import subprocess
import pytest
import shutil

tmpfile = './tests/tmp.m';

# uintTypes = ["uint8", "uint16", "uint32"]
intTypes = ["int8", "int16", "int32"]
floatTypes = ["float16", "float32", "float64"]

def returnArray(array):
    for x in array:
        yield x

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

@pytest.mark.parametrize("intType" , returnArray(intTypes))
def test_arithmeticIntOpertaions(intType):
    arithmetiTest(intType, "100+2", "102")