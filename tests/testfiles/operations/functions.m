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

    int16 d = 95;
    int16 f = 10;

    test2(d -> f);
}


func test2(int16 d, int16 f)
{
    int16 fsd = 5;
    print(fsd);
}
