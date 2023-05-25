#define MUNIT_ENABLE_ASSERT_ALIASES
#include "./lib/munit.h"
#include "./include/type2text.h"
#include "./include/symbol_table.h"


MunitResult test_print_type(const MunitParameter params[], void* user_data_or_fixture) {
    assert_string_equal(printType(272), "root");
    assert_string_equal(printType(274), "setup");
    assert_string_equal(printType(275), "mainloop");
    assert_string_equal(printType(293), "funcs");
    assert_string_equal(printType(276), "func");
    assert_string_equal(printType(296), "funccall");
    assert_string_equal(printType(295), "params");
    assert_string_equal(printType(294), "param");
    assert_string_equal(printType(290), "lines");
    assert_string_equal(printType(302), "vardecl");
    assert_string_equal(printType(292), "control");
    assert_string_equal(printType(289), "print");

    /* Control structures */
    assert_string_equal(printType(304), "while");
    assert_string_equal(printType(305), "if");

    /* Logical operators */
    assert_string_equal(printType(266), "lor");
    assert_string_equal(printType(267), "land");

    /* Compare operators */
    assert_string_equal(printType(261), "greater eq");
    assert_string_equal(printType(260), "less eq");
    assert_string_equal(printType(262), "eq");
    assert_string_equal(printType(263), "not eq");
    assert_string_equal(printType(264), "less");
    assert_string_equal(printType(265), "greater");

    /* Arithmetic operations */
    assert_string_equal(printType(283), "plus");
    assert_string_equal(printType(284), "minus");
    assert_string_equal(printType(285), "times");
    assert_string_equal(printType(286), "div");

    /* Expression */
    assert_string_equal(printType(298), "term");
    assert_string_equal(printType(299), "factor");

    /* Types */
    assert_string_equal(printType(269), "identifier");
    assert_string_equal(printType(258), "integer");
    assert_string_equal(printType(259), "float");



    assert_string_equal(printType(297), "empty");
    assert_string_equal(printType(2), "not implemented");
    assert_string_equal(printType(-5), "not implemented");
    return MUNIT_OK;
}

MunitResult test_get_custom_type(const MunitParameter params[], void* user_data_or_fixture) {
    assert_string_equal(getCustomType(2), "int8");
    assert_string_equal(getCustomType(3), "int16");
    assert_string_equal(getCustomType(4), "int32");
    assert_string_equal(getCustomType(5), "uint8");
    assert_string_equal(getCustomType(6), "uint16");
    assert_string_equal(getCustomType(7), "uint32");
    assert_string_equal(getCustomType(8), "float16");
    assert_string_equal(getCustomType(9), "float32");
    assert_string_equal(getCustomType(10), "float64");
    return MUNIT_OK;
}

MunitResult test_type_converter(const MunitParameter params[], void* user_data_or_fixture) {
    assert_string_equal(typeConverter(2, 0), "i8");
    assert_string_equal(typeConverter(3, 0), "i16");
    assert_string_equal(typeConverter(4, 0), "i32");
    assert_string_equal(typeConverter(5, 0), "i8");
    assert_string_equal(typeConverter(6, 0), "i16");
    assert_string_equal(typeConverter(7, 0), "i32");
    assert_string_equal(typeConverter(8, 0), "half");
    assert_string_equal(typeConverter(9, 0), "float");
    assert_string_equal(typeConverter(10, 0), "double");
    return MUNIT_OK;
}

MunitResult test_symbol_table(const MunitParameter params[], void* user_data_or_fixture) {
    // Crate a new symbol table
    struct HashTables* symTable = createMainTable(100);
    // Change scope to setup
    changeScope("setup");

    // Ensure scope value is set correctly
    assert_int(symTable->scope, ==, 0);

    // Create a new symbol
    createSymbol("TestVariable", int16_enum);

    // Search for symbol created in table
    struct searchReturn searchResult = searchSymbol(symTable->hTable[symTable->scope], "TestVariable");
    if (searchResult.type == not_found_enum) {
        return MUNIT_FAIL;
    } else {
        // Ensure the symbol is the correct type
        assert_int(searchResult.type, ==, int16_enum);
    }

    // Change scope and search global scope for symbol
    changeScope("mainloop");

    // Ensure scope value is set correctly
    assert_int(symTable->scope, ==, 1);

    searchResult = searchSymbol(symTable->hTable[0], "TestVariable");
    if (searchResult.type == not_found_enum) {
        return MUNIT_FAIL;
    }

    return MUNIT_OK;
}

MunitResult test_symbol_table_hash(const MunitParameter params[], void* user_data_or_fixture) {
    // Crate a new symbol table
    struct HashTables* symTable = createMainTable(100);
    // Change scope to setup
    changeScope("setup");

    int hashedValue = hash(symTable->hTable[symTable->scope], "test");
    assert_int(hashedValue, ==, 985);


    hashedValue = hash(symTable->hTable[symTable->scope], "somelongstring");
    assert_int(hashedValue, ==, 2068);
}

static MunitTest test_suite_tests[] = {
    { (char*) "type2text printType", test_print_type, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "type2text getCustomType", test_get_custom_type, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "type2text typeConverter", test_type_converter, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "symbol_table create and search", test_symbol_table, MUNIT_TEST_OPTION_NONE, NULL },
    { (char*) "symbol_table hash", test_symbol_table_hash, MUNIT_TEST_OPTION_NONE, NULL },
};


static const MunitSuite test_suite = {
    (char*) "",
    test_suite_tests,
    NULL,
    1,
    MUNIT_SUITE_OPTION_NONE
};

int main(int argc, char* argv[MUNIT_ARRAY_PARAM(argc + 1)]) {
    return munit_suite_main(&test_suite, (void*) "µnit", argc, argv);
}