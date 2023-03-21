// test4 checks if else if, testfail should be undeclared and testaccept should be 2

setup(){
  bool testif = false;
}
mainloop(){
  if (testif) {
    int8 testfail = 1;
  } else if (true) {
    int8 testaccept = 2;
  }
}