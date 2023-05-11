setup()
{
}
mainloop(){
  uint16 a = 2;
  uint16 b = 2;
  uint16 c = 2;
  uint16 d = 2;
  uint16 i = 0;
  while (i <= 10)
  {
    a = b + c;
    d = a * i;
    i = i + 1;
    
  }
  print(c);
}