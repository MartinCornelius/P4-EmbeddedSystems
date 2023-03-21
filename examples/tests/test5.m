// test5 checks if else if else, testfail1 and testfail2 should be undeclared, testaccept should be 3

setup()
{
    bool testif = false;
}
mainloop()
{
  if (testif) {
    int8 testfail1 = 1;
  } else if (false && true) {
    int8 testfail2 = 2;
  } else {
    int8 testaccept = 3;
  }
}