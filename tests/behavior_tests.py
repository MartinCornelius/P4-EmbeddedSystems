import subprocess
import unittest

class BehaviorTests(unittest.TestCase):
    #------------ ASSIGNMENTS ------------
    # 1 Uint8 a = 7
    def test_uint8AssignmentConstant(self):
        expected = ["7"]
        subprocess.run(["./run", "tests/testfiles/testfile1.m"])
        result = subprocess.run(["lli", "output/example_program.ll"], capture_output=True, text=True)
        self.assertEqual(expected, result.stdout.split())

    # 2 Uint8 Plus expression a = 2 + 3
    def test_uint8AssignmentPlusExpression(self):
        expected = ["5"]
        subprocess.run(["./run", "tests/testfiles/testfile2.m"])
        result = subprocess.run(["lli", "output/example_program.ll"], capture_output=True, text=True)
        self.assertEqual(expected, result.stdout.split())

    # 3 Uint8 Minus expression a = 8 - 4
    def test_uint8AssignmentPlusExpression(self):
        expected = ["4"]
        subprocess.run(["./run", "tests/testfiles/testfile3.m"])
        result = subprocess.run(["lli", "output/example_program.ll"], capture_output=True, text=True)
        self.assertEqual(expected, result.stdout.split())

    # 4 Uint8 Assignment to expression a = a + 1
    def test_uint8AssignmentPlusExpression(self):
        expected = ["3"]
        subprocess.run(["./run", "tests/testfiles/testfile4.m"])
        result = subprocess.run(["lli", "output/example_program.ll"], capture_output=True, text=True)
        self.assertEqual(expected, result.stdout.split())

    # Uint8 While a (= 1) < 10
    def test_uint8WhileLessThan(self):
        expected = ["10"]
        subprocess.run(["./run", "tests/testfiles/testfile5.m"])
        result = subprocess.run(["lli", "output/example_program.ll"], capture_output=True, text=True)
        self.assertEqual(expected, result.stdout.split())


if __name__ == '__main__':
    unittest.main()