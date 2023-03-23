#include <stdio.h>
#include <stdbool.h>

#include "CuTest.h"

#include "../src/include/parser.h"

void TestCOPEquals(CuTest* tc)
{
    int a = 10;
    int b = 10;

    int actual = cOp(a, "==", b);
    int expected = true; 

    CuAssertIntEquals(tc, expected, actual);
}

void TestCOPFailEquals(CuTest* tc)
{
    int a = 10;
    int b = 15;

    int actual = cOp(a, "==", b);
    int expected = false;

    CuAssertIntEquals(tc, expected, actual);
}

CuSuite* ParserTestGetSuite()
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, TestCOPEquals);
    SUITE_ADD_TEST(suite, TestCOPFailEquals);

    return suite;
}