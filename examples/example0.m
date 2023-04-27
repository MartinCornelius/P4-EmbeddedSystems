setup()
{
}
mainloop(){
  uint16 a = 2;
  uint16 c = 3;
  while (a > 10)
  {
    uint16 b = 5 + a;
    while (a < 5)
    {
      c = b + b;
    }
  }
  print(c);
}

