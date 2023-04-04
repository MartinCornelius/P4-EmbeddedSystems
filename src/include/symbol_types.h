#ifndef SYMBOLTYPES_H
#define SYMBOLTYPES_H

enum types { not_found_enum = -1, input_enum, output_enum, int8_enum, int16_enum, int32_enum, uint8_enum, 
                 uint16_enum, uint32_enum, float_enum, bool_enum, char_enum, flexint_enum };

struct Symbol{
  char name[32];
  void *next;
  int value;
  int type;     // -1
};
typedef struct Symbol Symbol_Struct;

struct input_symbol {
  char name[32];
  void *next;
  int value;    // 0x45 hex work in c, print as hex with %x
  int type;     // 0
};
typedef struct input_symbol input_Struct;

struct output_symbol {
  char name[32];
  void *next;
  int value;    // hex
  int type;     // 1
};
typedef struct output_symbol output_Struct;

struct int8_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //2
};
typedef struct int8_symbol int8_Struct;

struct int16_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //3
};
typedef struct int16_symbol int16_Struct;

struct uint8_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //4
};
typedef struct uint8_symbol uint8_Struct;

struct uint16_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //5
};
typedef struct uint16_symbol uint16_Struct;

struct float8_symbol {
  char name[32];
  void *next;
  float value;
  int type;     //6
};
typedef struct float8_symbol float8_Struct;

struct float16_symbol {
  char name[32];
  void *next;
  float value;
  int type;     //7
};
typedef struct float16_symbol float16_Struct;

struct bool_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //8
};
typedef struct bool_symbol bool_Struct;

struct char_symbol {
  char name[32];
  void *next;
  int type;
  char value;   //9
};
typedef struct char_symbol char_Struct;

struct flexint_symbol {
  char name[32];
  void *next;
  int value;
  int type;     //10
};
typedef struct flexint_symbol flexint_Struct;

#endif