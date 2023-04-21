setup()
{
}

mainloop()
{
  uint16 a = 1;

  while (a < 5)
  {
    uint16 b = a;
    while (a < 10)
    {
      uint16 c = b;
      a = a + 1;
    }
    a = a + 1;
  }
  print(c);
}