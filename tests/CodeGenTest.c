#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "../src/include/symbol_types.h"
#include "../src/include/code_gen.h"
#include "CuTest.h"

void EmitAppendString(CuTest* tc)
{
    char actual[128] = "123";
    char* expected = "123456";

    emit("456", actual);

    CuAssertStrEquals(tc, expected, actual);
}

void TypeToStringEqualsInt8t(CuTest* tc)
{
    int type = int8_enum;
    char testInput[20];

    char* expected = "int8_t";
    typeToString(testInput, type);

    CuAssertStrEquals(tc, expected, testInput);
}

void TypeToStringNotEqualsInt8t(CuTest* tc)
{
    int type = float_enum;
    char testInput[20];

    char* notExpected = "int8_t";
    typeToString(testInput, type);

    // strcmp returns 0 if inputs are equal
    int result = strcmp(notExpected, testInput);

    CuAssertTrue(tc, result);
}

void TypeToStringEqualsFloat(CuTest* tc)
{
    int type = float_enum;
    char testInput[20];

    char* expected = "float";
    typeToString(testInput, type);

    CuAssertStrEquals(tc, expected, testInput);
}

void TypeToStringNotEqualsFloat(CuTest* tc)
{
    int type = int8_enum;
    char testInput[20];

    char* notExpected = "float";
    typeToString(testInput, type);

    // strcmp returns 0 if inputs are equal
    int result = strcmp(notExpected, testInput);

    CuAssertTrue(tc, result);
}

void TypeToStringEqualsDefaultFlexInt(CuTest* tc)
{
    int type = 87;
    char testInput[20];

    char* expected = "flexint";
    
    typeToString(testInput, type);

    CuAssertStrEquals(tc, expected, testInput);
}


void TypeToStringNotEqualsDefaultFlexInt(CuTest* tc)
{
    int type = int8_enum;
    char testInput[20];

    char* notExpected = "flexint";
    typeToString(testInput, type);

    // strcmp returns 0 if inputs are equal
    int result = strcmp(notExpected, testInput);

    CuAssertTrue(tc, result);
}

CuSuite* CodeGenTestGetSuite()
{
    CuSuite* suite = CuSuiteNew();

    SUITE_ADD_TEST(suite, EmitAppendString);
    SUITE_ADD_TEST(suite, TypeToStringEqualsInt8t);
    SUITE_ADD_TEST(suite, TypeToStringNotEqualsInt8t);
    SUITE_ADD_TEST(suite, TypeToStringEqualsFloat);
    SUITE_ADD_TEST(suite, TypeToStringNotEqualsFloat);
    SUITE_ADD_TEST(suite, TypeToStringEqualsDefaultFlexInt);
    SUITE_ADD_TEST(suite, TypeToStringNotEqualsDefaultFlexInt);
    return suite;
}