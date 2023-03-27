// test13 small test program for 2 defines 2 funcs 6 vardecls and 2 ass expr in mainloop for format testing purposes

define FirstDefine 1
define SndDefind 2

func testfunc(int8 a -> int8 b){
    b = a + 4;
    b <- b;
}
func test2func2(int8 c, float8 d -> int8 e, float8 f){
  e <- c + d;
  f <- d - c;
}

setup(){
  int8 a = FirstDefine;
  int8 b = SndDefind;
  int8 c = 3;
  float8 d = 4;
  int8 e = 5;
  float8 f = 6;
}

mainloop(){
  a = b + c;
  d = e + f;
}