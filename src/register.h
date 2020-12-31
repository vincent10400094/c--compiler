#ifndef __REGISTER_H__
#define __REGISTER_H__

#include <stdio.h>
#include <stdlib.h>

typedef struct {
  short number;
  char dirty;
  char used;
  struct SymbolTableEntry *entry;
  int ref_count;
} Register;

typedef enum RegType {
  INT_T,
  INT_S,
  FLOAT_T,
  FLOAT_S
} RegType;

extern FILE *fp;

extern Register reg_int[32];
extern Register reg_float[32];

extern int int_t_reg_list[7];
extern int int_s_reg_list[10];

extern int float_t_reg_list[12];
extern int float_s_reg_list[12];

int GetReg(RegType reg_type);
void FreeReg(int reg_number, RegType reg_type);
void InitRegs();
void StoreStaticVariable(int reg_number, RegType reg_type);
void StoreLocalVariable(int reg_number, RegType reg_type);
void StoreStaticVariables();
void FreeSavedRegisters();

int CheckINT_T(int reg);
int CheckINT_S(int reg);
int CheckFLOAT_T(int reg);
int CheckFLOAT_S(int reg);

void PrintRegUsage();

#endif