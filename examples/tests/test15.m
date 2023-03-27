// test15 print and pointers

func b(int8 a -> int8 c){
  c <- a + 2;
}
setup(){
  int8 a = 2;
  int8 c;
}

mainloop(){
  a = 4;
  print("%d\n", a);
  b(a -> c);
  print("%d\n", a);
  print("%d\n", c);
}