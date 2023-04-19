// test scope is meant to test the scope via the symbol table

define TEST1 3 + 2 * 5



setup()
{
  int16 setup = TEST1;
}

mainloop()
{
  int16 mainloop = 10;

  test();
}


func test() {
  int16 functest = 0;
}
