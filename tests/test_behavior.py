import subprocess
import unittest
import shutil

tmpfile = './tests/tmp.m';

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

class BehaviorTests(unittest.TestCase):
    def test_arithmeticOpertaions(self):
        exprType = "uint8";
        operation = "7 + 9";

        try:
            expect = str(eval(operation));

            filedata = fileCopyAndOpen('./tests/testfiles/arithmetic.m');
            filedata = fileReplacement(filedata, 'exprType', exprType);
            filedata = fileReplacement(filedata, 'expression', expect);
            writeToTmp(filedata);

            subprocess.run(["./run", tmpfile], check=True)
            result = subprocess.run(["lli", "./output/example_program.ll"], stdout=subprocess.PIPE, check=True)
        except Exception as e:
            print(e)
        else:
            self.assertEqual(expect, result.stdout.decode("utf-8").replace("\n", ""))

if __name__ == '__main__':
    unittest.main()