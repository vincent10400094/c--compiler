#include "register.h"

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "symbolTable.h"

// register parts
Register reg_int[32];
Register reg_float[32];

int int_t_reg_list[7] = {5, 6, 7, 28, 29, 30, 31};
int int_s_reg_list[10] = {18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

int float_t_reg_list[12] = {0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 30, 31};
int float_s_reg_list[12] = {8, 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

int iconst_label_number_s = 1;

void InitRegs() {
  for (int i = 0; i < 32; i++) {
    reg_int[i].number = i;
    reg_int[i].dirty = 0;
    reg_int[i].used = 0;
    reg_int[i].ref_count = 0;
    reg_float[i].number = i;
    reg_float[i].dirty = 0;
    reg_float[i].used = 0;
    reg_float[i].ref_count = 0;
  }
}

int cnt_int = 0, cnt_float;

int GetRegToFree(RegType reg_type) {
  assert(reg_type == INT_S || reg_type == FLOAT_S);
  if (reg_type == INT_S) {
    if (cnt_int >= 10)
      cnt_int = 0;
    return int_s_reg_list[cnt_int++];
  } else {
    if (cnt_float >= 12)
      cnt_float = 0;
    return float_s_reg_list[cnt_float++];
  }
}

void PrintRegUsage() {
  puts("================[Register Usage]==================");
  for (int i = 0; i < 10; i++)
    if (reg_int[int_s_reg_list[i]].used)
      printf("- x%d associated with %s, dirty=%s, ref_count=%d\n", int_s_reg_list[i], reg_int[int_s_reg_list[i]].entry->name, reg_int[int_s_reg_list[i]].dirty ? "True" : "Flase", reg_int[int_s_reg_list[i]].ref_count);
  for (int i = 0; i < 12; i++)
    if (reg_float[float_s_reg_list[i]].used)
      printf("- f%d associated with %s, dirty=%s, ref_count=%d\n", float_s_reg_list[i], reg_float[float_s_reg_list[i]].entry->name, reg_float[float_s_reg_list[i]].dirty ? "True" : "Flase", reg_float[float_s_reg_list[i]].ref_count);
}

int GetReg(RegType reg_type) {
  switch (reg_type) {
    case INT_T: {
      for (int i = 0; i < 7; i++) {
        if (!reg_int[int_t_reg_list[i]].used) {
          reg_int[int_t_reg_list[i]].used = 1;
          return int_t_reg_list[i];
        }
      }
      break;
    }
    case INT_S: {
      for (int i = 0; i < 10; i++) {
        if (!reg_int[int_s_reg_list[i]].used) {
          reg_int[int_s_reg_list[i]].used = 1;
          return int_s_reg_list[i];
        }
      }
      // TODO: all registrs are in-use
      int reg_to_free = GetRegToFree(INT_S);
      FreeReg(reg_to_free, INT_S);
      reg_int[reg_to_free].used = 1;
      return reg_to_free;
    }
    case FLOAT_T: {
      for (int i = 0; i < 12; i++) {
        if (!reg_float[float_t_reg_list[i]].used) {
          reg_float[float_t_reg_list[i]].used = 1;
          return float_t_reg_list[i];
        }
      }
      break;
    }
    case FLOAT_S: {
      for (int i = 0; i < 12; i++) {
        if (!reg_float[float_s_reg_list[i]].used) {
          reg_float[float_s_reg_list[i]].used = 1;
          return float_s_reg_list[i];
          printf("%d\n", float_s_reg_list[i]);
        }
      }
      // TODO: all registrs are in-use
      int reg_to_free = GetRegToFree(FLOAT_S);
      printf("free f%d\n", reg_to_free);
      FreeReg(reg_to_free, FLOAT_S);
      reg_float[reg_to_free].used = 1;
      return reg_to_free;
    }
  }
  fprintf(stderr, "This should not happen, run out of registers\n");
  exit(1);
}

int CheckINT_T(int reg) {
  return (reg >= 5 && reg <= 7) || (reg >= 28);
}

int CheckINT_S(int reg) {
  return reg >= 18 && reg <= 27;
}

int CheckFLOAT_T(int reg) {
  return reg <= 7 || reg >= 28;
}

int CheckFLOAT_S(int reg) {
  return (reg >= 8 && reg <= 9) || (reg >= 18 && reg <= 27);
}

void FreeReg(int reg_number, RegType reg_type) {
  switch (reg_type) {
    case INT_T: {
      if (!CheckINT_T(reg_number))
        return;
      reg_int[reg_number].used = 0;
      break;
    }
    case INT_S: {
      if (!CheckINT_S(reg_number))
        return;
      //   printf("free x%d\n", reg_number);
      if (reg_int[reg_number].dirty) {
        if (reg_int[reg_number].entry->scope == 0) {
          StoreStaticVariable(reg_number, reg_type);
        } else {
          StoreLocalVariable(reg_number, reg_type);
        }
      }
      reg_int[reg_number].entry->attribute->attr.typeDescriptor->reg = 0;
      reg_int[reg_number].dirty = 0;
      reg_int[reg_number].ref_count = 0;
      reg_int[reg_number].used = 0;
      break;
    }
    case FLOAT_T: {
      if (!CheckFLOAT_T(reg_number))
        return;
      reg_float[reg_number].used = 0;
      break;
    }
    case FLOAT_S: {
      if (!CheckFLOAT_S(reg_number))
        return;
      //   printf("free f%d\n", reg_number);
      if (reg_float[reg_number].dirty) {
        if (reg_float[reg_number].entry->scope == 0) {
          StoreStaticVariable(reg_number, reg_type);
        } else {
          StoreLocalVariable(reg_number, reg_type);
        }
      }
      reg_float[reg_number].entry->attribute->attr.typeDescriptor->reg = 0;
      reg_float[reg_number].dirty = 0;
      reg_float[reg_number].ref_count = 0;
      reg_float[reg_number].used = 0;
      break;
    }
  }
}

void StoreStaticVariable(int reg_number, RegType reg_type) {
  assert(reg_type == INT_S || reg_type == FLOAT_S);
  int tmp_reg = GetReg(INT_T);
  if (reg_type == INT_S) {
    SymbolTableEntry *entry = reg_int[reg_number].entry;
    fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, entry->name);
    fprintf(fp, "\tsw\tx%d,0(x%d)\n", reg_number, tmp_reg);
    reg_int[reg_number].dirty = 0;
  } else if (reg_type == FLOAT_S) {
    SymbolTableEntry *entry = reg_float[reg_number].entry;
    fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, entry->name);
    fprintf(fp, "\tfsw\tf%d,0(x%d)\n", reg_number, tmp_reg);
    reg_float[reg_number].dirty = 0;
  }
  FreeReg(tmp_reg, INT_T);
}

void StoreLocalVariable(int reg_number, RegType reg_type) {
  assert(reg_type == INT_S || reg_type == FLOAT_S);
  if (reg_type == INT_S) {
    if (reg_int[reg_number].entry->attribute->attr.typeDescriptor->offset >= 4096) {
      fprintf(fp, ".data\n");
      fprintf(fp, ".ICS%d: .word %d\n", iconst_label_number_s, reg_int[reg_number].entry->attribute->attr.typeDescriptor->offset);
      fprintf(fp, ".text\n");
      int tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tla\tx%d, .ICS%d\n", tmp_reg, iconst_label_number_s);
      fprintf(fp, "\tlw\tx%d,0(x%d)\n", tmp_reg, tmp_reg);
      fprintf(fp, "\tsub\tx%d,fp,x%d\n", tmp_reg, tmp_reg);
      fprintf(fp, "\tsw\tx%d,0(x%d)\n", reg_number, tmp_reg);
      FreeReg(tmp_reg, INT_T);
      iconst_label_number_s++;
    } else {
      fprintf(fp, "\tsw\tx%d,-%d(fp)\n", reg_number, reg_int[reg_number].entry->attribute->attr.typeDescriptor->offset);
    }
    reg_int[reg_number].dirty = 0;
  } else if (reg_type == FLOAT_S) {
    if (reg_float[reg_number].entry->attribute->attr.typeDescriptor->offset >= 4096) {
      fprintf(fp, ".data\n");
      fprintf(fp, ".ICS%d: .word %d\n", iconst_label_number_s, reg_float[reg_number].entry->attribute->attr.typeDescriptor->offset);
      fprintf(fp, ".text\n");
      int tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tla\tx%d, .ICS%d\n", tmp_reg, iconst_label_number_s);
      fprintf(fp, "\tlw\tx%d,0(x%d)\n", tmp_reg, tmp_reg);
      fprintf(fp, "\tsub\tx%d,fp,x%d\n", tmp_reg, tmp_reg);
      fprintf(fp, "\tfsw\tf%d,0(x%d)\n", reg_number, tmp_reg);
      FreeReg(tmp_reg, INT_T);
      iconst_label_number_s++;
    } else {
      fprintf(fp, "\tfsw\tf%d,-%d(fp)\n", reg_number, reg_float[reg_number].entry->attribute->attr.typeDescriptor->offset);
    }
    reg_float[reg_number].dirty = 0;
  }
}

void StoreStaticVariables() {
  for (int i = 0; i < 10; i++) {
    int reg_number = int_s_reg_list[i];
    if (reg_int[reg_number].dirty && reg_int[reg_number].entry->scope == 0) {
      // StoreStaticVariable(reg_number, INT_S);
      FreeReg(reg_number, INT_S);
    }
  }
  for (int i = 0; i < 12; i++) {
    int reg_number = float_s_reg_list[i];
    if (reg_float[reg_number].dirty && reg_float[reg_number].entry->scope == 0) {
      // StoreStaticVariable(reg_number, FLOAT_S);
      FreeReg(reg_number, FLOAT_S);
    }
  }
}

void FreeSavedRegisters() {
  for (int i = 0; i < 10; i++) {
    int reg_number = int_s_reg_list[i];
    if (reg_int[reg_number].used) {
      FreeReg(reg_number, INT_S);
    }
  }
  for (int i = 0; i < 12; i++) {
    int reg_number = float_s_reg_list[i];
    if (reg_float[reg_number].used) {
      FreeReg(reg_number, FLOAT_S);
    }
  }
}