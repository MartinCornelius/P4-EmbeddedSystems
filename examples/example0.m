setup()
{
  uint16 c = 2;
}
mainloop(){
  uint16 a = 2;
  uint16 c = 3;
  while (a > 10)
  {
    uint16 b = 5 + a;
    while (a < 5)
    {
      c = b;
    }
  }
  print(c);
}