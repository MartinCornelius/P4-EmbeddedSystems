typedef struct input input_Struct;

struct input {
  char type[9];
  char name[32];
  int value;    // 0x45 hex work in c, print as hex with %x
  void *next;
};

typedef struct output output_Struct;

struct output {
  char type[9];
  char name[32];
  int value;    // hex
  void *next;
};

typedef struct int8 int8_Struct;

struct int8 {
  char type[9];
  char name[32];
  int value;
  void *next;
};

typedef struct int16 int16_Struct;

struct int16 {
  char type[9];
  char name[32];
  int value;
  void *next;
};

typedef struct uint8 uint8_Struct;

struct uint8 {
  char type[9];
  char name[32];
  int value;
  void *next;
};

typedef struct uint16 uint16_Struct;

struct uint16 {
  char type[9];
  char name[32];
  int value;
  void *next;
};

typedef struct float8 float8_Struct;

struct float8 {
  char type[9];
  char name[32];
  float value;
  void *next;
};

typedef struct float16 float16_Struct;

struct float16 {
  char type[9];
  char name[32];
  float value;
  void *next;
};

typedef struct bool bool_Struct;

struct bool {
  char type[9];
  char name[32];
  int value;
  void *next;
};

typedef struct char char_Struct;

struct char {
  char type[9];
  char name[32];
  char value;
  void *next;
};

//flexint ;D
typedef struct int int_Struct;

struct int {
  char type[9];
  char name[32];
  int value;
  void *next;
};