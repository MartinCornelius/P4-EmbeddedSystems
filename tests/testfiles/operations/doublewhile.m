setup()
{
}

mainloop()
{
  int32 a = 0;

  while (a < 5)
  {
    while (a < 10)
    {
      a = a + 1;
    }
    a = a + 1;
  }
  print(a);
}