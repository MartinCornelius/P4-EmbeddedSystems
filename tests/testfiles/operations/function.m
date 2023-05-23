setup()
{
}

mainloop()
{
  int32 a = 12;
  int32 b = 58;
  test(a -> b);
}

func test(int32 a, int32 b)
{
    a = 500 + a + b;
    print(a);
}