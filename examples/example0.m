setup()
{
}

mainloop()
{
  int8 a = 12;
  int8 b = 55;
  redbull(a -> b);
  print(b);
}

func redbull(int8 a, int8 b)
{
    print(a);
    a = 2 + a;
    print(a);
}