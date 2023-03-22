struct input_symbol {
  char name[32];
  void *next;
  int value;    // 0x45 hex work in c, print as hex with %x
  int type;
};
typedef struct input_symbol input_Struct;

struct output_symbol {
  char name[32];
  void *next;
  int value;    // hex
  int type;
};
typedef struct output_symbol output_Struct;

struct int8_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct int8_symbol int8_Struct;

struct int16_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct int16_symbol int16_Struct;

struct uint8_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct uint8_symbol uint8_Struct;

struct uint16_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct uint16_symbol uint16_Struct;

struct float8_symbol {
  char name[32];
  void *next;
  float value;
  int type;
};
typedef struct float8_symbol float8_Struct;

struct float16_symbol {
  char name[32];
  void *next;
  float value;
  int type;
};
typedef struct float16_symbol float16_Struct;

struct bool_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct bool_symbol bool_Struct;

struct char_symbol {
  char name[32];
  void *next;
  int type;
  char value;
};
typedef struct char_symbol char_Struct;

struct flexint_symbol {
  char name[32];
  void *next;
  int value;
  int type;
};
typedef struct flexint_symbol flexint_Struct;
