setup()
{
}

mainloop(){
  uint16 a = 2;
  uint16 b = 2;
  uint16 c = 2;
  uint16 d = 2;
  uint16 i = 0;
  uint16 k = 0;

  while (k <= 10000){
    while (i <= 10000)
    {
    a = b + c;
    d = a * i;
    i = i + 1; 
   }
   i = i - i;

   k = k + 1;
  }


  print(c);
}