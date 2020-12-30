#include <assert.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"

// register parts
Register reg_int[32];
Register reg_float[32];

int int_t_reg_list[7] = {5, 6, 7, 28, 29, 30, 31};
int int_s_reg_list[10] = {18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

int float_t_reg_list[12] = {0, 1, 2, 3, 4, 5, 6, 7, 28, 29, 30, 31};
int float_s_reg_list[12] = {8, 9, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27};

int GetReg(RegType reg_type);
void FreeReg(int reg_number, RegType reg_type);
void InitRegs();
void StoreStaticVariable(int reg_number, RegType reg_type);
void StoreLocalVariable(int reg_number, RegType reg_type);
void StoreStaticVariables();

// global variables for labels
int AR_offset = 0;
int max_label_number = 1;
int sc_label_number = 1;
int fconst_label_number = 1;
int normal_label = 1;

FILE *fp;

void GenPrologue(char *function_name);
void GenEpilogue(char *function_name);
int GetOffset();
void ResetOffset();
void AllocateSymbol(SymbolTableEntry *entry, int size);
void GenSymbolDeclaration(AST_NODE *declaration_list_node);
void GenBlockNode(AST_NODE *block_node);
void GenStatement(AST_NODE *statement_list_node);
void GenFunctionDeclaration(AST_NODE *declaration_node);
void GenSymbolReference();
int GenExpr(AST_NODE *expr_node);
void GenAssignment(AST_NODE *assignment_node);
void GenReturnStmt(AST_NODE *return_node);
void PassParameter();
void GenFunctionCall(AST_NODE *stmt_node);
void GenIfStmt(AST_NODE *stmt_node);
void GenWhileStmt(AST_NODE *stmt_node);
void symbolTableAdd(char *symbol_name);
void GenHead(AST_NODE *id_name_node);
void CodeGen(AST_NODE *root, FILE *fp);
int GenVp(AST_NODE *id_node);
int LoadVariable(AST_NODE *id_node);

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
      printf("x%d associated with %s, dirty=%s, ref_count=%d\n", int_s_reg_list[i], reg_int[int_s_reg_list[i]].entry->name, reg_int[int_s_reg_list[i]].dirty ? "True" : "Flase", reg_int[int_s_reg_list[i]].ref_count);
  for (int i = 0; i < 12; i++)
    if (reg_float[float_s_reg_list[i]].used)
      printf("f%d associated with %s, dirty=%s, ref_count=%d\n", float_s_reg_list[i], reg_float[float_s_reg_list[i]].entry->name, reg_float[float_s_reg_list[i]].dirty ? "True" : "Flase", reg_float[float_s_reg_list[i]].ref_count);
  puts("================[End]==================");
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
      printf("free x%d\n", reg_number);
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
      printf("free f%d\n", reg_number);
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
  SymbolTableEntry *entry = reg_int[reg_number].entry;
  int tmp_reg = GetReg(INT_T);
  fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, entry->name);
  if (entry->attribute->attr.typeDescriptor->properties.dataType == INT_TYPE) {
    fprintf(fp, "\tsw\tx%d,0(x%d)\n", reg_number, tmp_reg);
  } else if (entry->attribute->attr.typeDescriptor->properties.dataType == FLOAT_TYPE) {
    fprintf(fp, "\tfsw\tf%d,0(x%d)\n", reg_number, tmp_reg);
  }
  FreeReg(tmp_reg, INT_T);
}

void StoreLocalVariable(int reg_number, RegType reg_type) {
  assert(reg_type == INT_S || reg_type == FLOAT_S);
  if (reg_type == INT_S) {
    fprintf(fp, "\tsw\tx%d,-%d(fp)\n", reg_number, reg_int[reg_number].entry->attribute->attr.typeDescriptor->offset);
  } else if (reg_type == FLOAT_S) {
    fprintf(fp, "\tfsw\tf%d,-%d(fp)\n", reg_number, reg_float[reg_number].entry->attribute->attr.typeDescriptor->offset);
  }
}

void StoreStaticVariables() {
  for (int i = 0; i < 10; i++) {
    int reg_number = int_s_reg_list[i];
    if (reg_int[reg_number].dirty && reg_int[reg_number].entry->scope == 0) {
      StoreStaticVariable(reg_number, INT_S);
    }
  }
  for (int i = 0; i < 12; i++) {
    int reg_number = float_s_reg_list[i];
    if (reg_float[reg_number].dirty && reg_float[reg_number].entry->scope == 0) {
      StoreStaticVariable(reg_number, FLOAT_S);
    }
  }
}

void StoreDirtyRegisters() {
  for (int i = 0; i < 10; i++) {
    int reg_number = int_s_reg_list[i];
    if (reg_int[reg_number].dirty) {
      if (reg_int[reg_number].entry->scope == 0)
        StoreStaticVariable(reg_number, INT_S);
      else
        StoreLocalVariable(reg_number, INT_S);
    }
  }
  for (int i = 0; i < 12; i++) {
    int reg_number = float_s_reg_list[i];
    if (reg_float[reg_number].dirty) {
      if (reg_float[reg_number].entry->scope == 0)
        StoreStaticVariable(reg_number, FLOAT_S);
      else
        StoreLocalVariable(reg_number, FLOAT_S);
    }
  }
}

int GetOffset() {
  return AR_offset;
}

void ResetOffset() {
  AR_offset = 0;
}

void AllocateSymbol(SymbolTableEntry *entry, int size) {
  assert(entry != NULL);
  AR_offset += size;
  entry->attribute->attr.typeDescriptor->offset = GetOffset();
}

void GenSymbolDeclaration(AST_NODE *declaration_list_node) {
  AST_NODE *declaration_node = declaration_list_node->child;
  while (declaration_node) {
    assert(declaration_node->nodeType == DECLARATION_NODE);
    if (declaration_node->semantic_value.declSemanticValue.kind == VARIABLE_DECL) {
      AST_NODE *type_node = declaration_node->child;
      AST_NODE *id_node = type_node->rightSibling;
      while (id_node) {
        switch (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->kind) {
          case SCALAR_TYPE_DESCRIPTOR: {
            if (id_node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
              if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
                assert(id_node->dataType != NONE_TYPE);
                if (id_node->dataType == INT_TYPE) {
                  fprintf(fp, "_g_%s: .word 0\n", id_node->semantic_value.identifierSemanticValue.identifierName);
                } else if (id_node->dataType == FLOAT_TYPE) {
                  fprintf(fp, "_g_%s: .float 0\n", id_node->semantic_value.identifierSemanticValue.identifierName);
                }
              } else {
                AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
              }
            } else if (id_node->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID) {
              if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
                AST_NODE *const_node = id_node->child;
                if (const_node->semantic_value.const1->const_type == INTEGERC) {
                  fprintf(fp, "_g_%s: .word %d\n", id_node->semantic_value.identifierSemanticValue.identifierName, const_node->semantic_value.const1->const_u.intval);
                } else if (const_node->semantic_value.const1->const_type == FLOATC) {
                  float fconst = const_node->semantic_value.const1->const_u.fval;
                  int float_to_int = *(int *)&fconst;
                  fprintf(fp, "_g_%s: .float %d\n", id_node->semantic_value.identifierSemanticValue.identifierName, float_to_int);
                }
              } else {
                AST_NODE *init_node = id_node->child;
                int rs = GenExpr(id_node->child);
                assert(init_node->dataType != NONE_TYPE);
                if (init_node->dataType == INT_TYPE) {
                  AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
                  fprintf(fp, "\tsw\tx%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
                  FreeReg(rs, INT_T);
                } else if (init_node->dataType == FLOAT_TYPE) {
                  AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
                  fprintf(fp, "\tfsw\tf%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
                  FreeReg(rs, FLOAT_T);
                }
              }
            }
            break;
          }
          case ARRAY_TYPE_DESCRIPTOR: {
            int total_size = 4;
            for (int i = 0; i < id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension; i++) {
              total_size *= id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
            }
            if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
              fprintf(fp, "_g_%s: .space %d\n", id_node->semantic_value.identifierSemanticValue.identifierName, total_size);
            } else {
              AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, total_size);
            }
            break;
          }
          default: {
            break;
          }
        }
        id_node = id_node->rightSibling;
      }
    }
    declaration_node = declaration_node->rightSibling;
  }
}

void GenPrologue(char *function_name) {
  fprintf(fp,
          "\tsd\tra,0(sp)\n\tsd\tfp,-8(sp)\n\tadd fp,sp,-8\n\tadd sp,sp,-16\n\tla\tra,_frameSize_%s\n\tlw\tra,0(ra)\n\t\
sub\tsp,sp,ra\n\tsd\tt0,8(sp)\n\tsd\tt1,16(sp)\n\tsd\tt2,24(sp)\n\tsd\tt3,32(sp)\n\tsd\tt4,40(sp)\n\tsd\tt5,48(sp)\n\t\
sd\tt6,56(sp)\n\tsd\ts2,64(sp)\n\tsd\ts3,72(sp)\n\tsd\ts4,80(sp)\n\tsd\ts5,88(sp)\n\tsd\ts6,96(sp)\n\tsd\ts7,104(sp)\n\t\
sd\ts8,112(sp)\n\tsd\ts9,120(sp)\n\tsd\ts10,128(sp)\n\tsd\ts11,136(sp)\n\tsd\tfp,144(sp)\n\t\
fsw\tft0,152(sp)\n\tfsw\tft1,156(sp)\n\tfsw\tft2,160(sp)\n\tfsw\tft3,164(sp)\n\tfsw\tft4,168(sp)\n\tfsw\tft5,172(sp)\n\t\
fsw\tft6,176(sp)\n\tfsw\tft7,180(sp)\n",
          function_name);
}

void GenEpilogue(char *function_name) {
  fprintf(fp, "_end_%s:\n", function_name);
  fprintf(fp,
          "\tld\tt0,8(sp)\n\tld\tt1,16(sp)\n\tld\tt2,24(sp)\n\tld\tt3,32(sp)\n\tld\tt4,40(sp)\n\tld\tt5,48(sp)\n\t\
ld\tt6,56(sp)\n\tld\ts2,64(sp)\n\tld\ts3,72(sp)\n\tld\ts4,80(sp)\n\tld\ts5,88(sp)\n\tld\ts6,96(sp)\n\tld\ts7,104(sp)\n\t\
ld\ts8,112(sp)\n\tld\ts9,120(sp)\n\tld\ts10,128(sp)\n\tld\ts11,136(sp)\n\tld\tfp,144(sp)\n\t\
flw\tft0,152(sp)\n\tflw\tft1,156(sp)\n\tflw\tft2,160(sp)\n\tflw\tft3,164(sp)\n\tflw\tft4,168(sp)\n\tflw\tft5,172(sp)\n\t\
flw\tft6,176(sp)\n\tflw\tft7,180(sp)\n\tld\tra,8(fp)\n\tmv\tsp,fp\n\tadd sp,sp,8\n\tld\tfp,0(fp)\n\tjr\tra\n.data\n");
  fprintf(fp, "_frameSize_%s: .word %d\n", function_name, 180 + AR_offset);
  AR_offset = 0;
}

void GenBlockNode(AST_NODE *block_node) {
  AST_NODE *node = block_node->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      GenSymbolDeclaration(node);
      fprintf(fp, ".text\n");
    } else if (node->nodeType == STMT_LIST_NODE) {
      AST_NODE *stmt_node = node->child;
      while (stmt_node) {
        GenStatement(stmt_node);
        stmt_node = stmt_node->rightSibling;
      }
    }
    node = node->rightSibling;
  }
}

void GenStatement(AST_NODE *stmt_node) {
  if (stmt_node->nodeType == STMT_NODE) {
    switch (stmt_node->semantic_value.stmtSemanticValue.kind) {
      case ASSIGN_STMT: {
        puts("Gen assign");
        GenAssignment(stmt_node);
        PrintRegUsage();
        break;
      }
      case IF_STMT: {
        GenIfStmt(stmt_node);
        break;
      }
      case FOR_STMT: {
        break;
      }
      case FUNCTION_CALL_STMT: {
        puts("Gen functionCall");
        GenFunctionCall(stmt_node);
        break;
      }
      case WHILE_STMT: {
        GenWhileStmt(stmt_node);
        break;
      }
      case RETURN_STMT: {
        GenReturnStmt(stmt_node);
        break;
      }
    }
  } else if (stmt_node->nodeType == BLOCK_NODE) {
    GenBlockNode(stmt_node);
  }
}

int GenVp(AST_NODE *id_node) {
  assert(id_node->semantic_value.identifierSemanticValue.kind == ARRAY_ID);
  AST_NODE *dimension_node = id_node->child;
  int vp = GenExpr(dimension_node);
  dimension_node = dimension_node->rightSibling;
  int dimension_count = 1;
  SymbolTableEntry *entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
  while (dimension_node) {
    int reg = GenExpr(dimension_node);
    int tmp_reg = GetReg(INT_T);
    fprintf(fp, "\tli\tx%d,%d\n", tmp_reg, entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[dimension_count]);
    fprintf(fp, "\tmul\tx%d,x%d,x%d\n", vp, tmp_reg, vp);
    fprintf(fp, "\tadd\tx%d,x%d,x%d\n", vp, reg, vp);
    dimension_node = dimension_node->rightSibling;
    FreeReg(reg, INT_T);
    FreeReg(tmp_reg, INT_T);
    dimension_count++;
  }
  int tmp_reg = GetReg(INT_T);
  fprintf(fp, "\tli\tx%d,%d\n", tmp_reg, 4);
  fprintf(fp, "\tmul\tx%d,x%d,x%d\n", vp, tmp_reg, vp);
  FreeReg(tmp_reg, INT_T);
  return vp;
}

// return value: register number that holds the value
int LoadVariable(AST_NODE *id_node) {
  SymbolTableEntry *entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
  assert(id_node->dataType != NONE_TYPE);
  int tmp_reg, reg;
  if (id_node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {  // variable
    // there is a saved register holds the variable's value
    if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->reg != 0) {
      reg = id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->reg;
      if (id_node->dataType == INT_TYPE)
        reg_int[reg].ref_count++;
      else if (id_node->dataType == FLOAT_TYPE)
        reg_float[reg].ref_count++;
      return reg;
    }
    if (entry->scope == 0) {  // static variable
      tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      FreeReg(tmp_reg, INT_T);
      if (id_node->dataType == INT_TYPE) {
        reg = GetReg(INT_S);
        fprintf(fp, "\tlw\tx%d,0(x%d)\n", reg, tmp_reg);
      } else if (id_node->dataType == FLOAT_TYPE) {
        reg = GetReg(FLOAT_S);
        fprintf(fp, "\tflw\tf%d,0(x%d)\n", reg, tmp_reg);
      }
    } else {  // local variable
      if (id_node->dataType == INT_TYPE) {
        reg = GetReg(INT_S);
        fprintf(fp, "\tlw\tx%d,-%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
      } else if (id_node->dataType == FLOAT_TYPE) {
        reg = GetReg(FLOAT_S);
        fprintf(fp, "\tflw\tf%d,-%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
      }
    }
    if (id_node->dataType == INT_TYPE) {
      reg_int[reg].entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
      reg_int[reg].entry->attribute->attr.typeDescriptor->reg = reg;
      reg_int[reg].ref_count = 1;
    } else {
      reg_float[reg].entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
      reg_float[reg].entry->attribute->attr.typeDescriptor->reg = reg;
      reg_float[reg].ref_count = 1;
    }
  } else {                    // array
    if (entry->scope == 0) {  // static variable
      tmp_reg = GetReg(INT_T);
      int vp = GenVp(id_node);
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      fprintf(fp, "\tadd\tx%d,x%d,x%d\n", tmp_reg, vp, tmp_reg);
      if (id_node->dataType == INT_TYPE) {
        reg = GetReg(INT_T);
        fprintf(fp, "\tlw\tx%d,0(x%d)\n", reg, tmp_reg);
      } else if (id_node->dataType == FLOAT_TYPE) {
        reg = GetReg(FLOAT_T);
        fprintf(fp, "\tflw\tf%d,0(x%d)\n", reg, tmp_reg);
      }
      FreeReg(tmp_reg, INT_T);
      FreeReg(vp, INT_T);
    } else {  // local variable
      int vp = GenVp(id_node);
      fprintf(fp, "\tadd\tx%d,fp,x%d\n", vp, vp);
      if (id_node->dataType == INT_TYPE) {
        reg = GetReg(INT_T);
        fprintf(fp, "\tlw\tx%d,-%d(x%d)\n", reg, entry->attribute->attr.typeDescriptor->offset, vp);
      } else if (id_node->dataType == FLOAT_TYPE) {
        reg = GetReg(FLOAT_T);
        fprintf(fp, "\tflw\tf%d,-%d(x%d)\n", reg, entry->attribute->attr.typeDescriptor->offset, vp);
      }
      FreeReg(vp, INT_T);
    }
  }
  return reg;
}

// return value: register number that holds the value
int GenExpr(AST_NODE *expr_node) {
  if (expr_node->nodeType == CONST_VALUE_NODE) {
    if (expr_node->semantic_value.const1->const_type == INTEGERC) {
      expr_node->dataType = INT_TYPE;
      int reg = GetReg(INT_T);
      fprintf(fp, "\tli\tx%d,%d\n", reg, expr_node->semantic_value.const1->const_u.intval);
      return reg;
    } else if (expr_node->semantic_value.const1->const_type == FLOATC) {
      expr_node->dataType = FLOAT_TYPE;
      fprintf(fp, ".data\n");
      float fconst = expr_node->semantic_value.const1->const_u.fval;
      int float_to_int = *(int *)&fconst;
      fprintf(fp, ".FC%d: .word %u\n", fconst_label_number, float_to_int);
      fprintf(fp, ".text\n");
      int tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tla\tx%d, .FC%d\n", tmp_reg, fconst_label_number);
      int reg = GetReg(FLOAT_T);
      fprintf(fp, "\tflw\tf%d,0(x%d)\n", reg, tmp_reg);
      FreeReg(tmp_reg, INT_T);
      fconst_label_number++;
      return reg;
    }
  } else if (expr_node->nodeType == IDENTIFIER_NODE) {
    if (expr_node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
      expr_node->dataType = expr_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.dataType;
    } else {
      expr_node->dataType = expr_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
    }
    int reg, tmp_reg;
    reg = LoadVariable(expr_node);
    assert(expr_node->dataType == INT_TYPE || expr_node->dataType == FLOAT_TYPE);
    if (expr_node->dataType == INT_TYPE) {
      tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tmv\tx%d,x%d\n", tmp_reg, reg);
    } else {
      tmp_reg = GetReg(FLOAT_T);
      fprintf(fp, "\tfmv.s\tf%d,f%d\n", tmp_reg, reg);
    }
    return tmp_reg;
  } else if (expr_node->nodeType == STMT_NODE) {
    // gen function call
    if ((expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry && expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.functionSignature->returnType == INT_TYPE) ||
        !strcmp(expr_node->child->semantic_value.identifierSemanticValue.identifierName, "read")) {
      GenFunctionCall(expr_node);
      int reg = GetReg(INT_T);
      fprintf(fp, "\tmv\tx%d,a0\n", reg);
      expr_node->dataType = INT_TYPE;
      return reg;
    } else {
      GenFunctionCall(expr_node);
      int reg = GetReg(FLOAT_T);
      fprintf(fp, "\tfmv.s\tf%d, fa0\n", reg);
      expr_node->dataType = FLOAT_TYPE;
      return reg;
    }
  }
  EXPRSemanticValue *semanticValue = &(expr_node->semantic_value.exprSemanticValue);
  if (semanticValue->kind == BINARY_OPERATION) {  // binary operation
    int rs1 = GenExpr(expr_node->child);
    int rs2 = GenExpr(expr_node->child->rightSibling);
    assert(expr_node->child->dataType != NONE_TYPE);
    // both children are INT_TYPE
    if (expr_node->child->dataType == INT_TYPE && expr_node->child->rightSibling->dataType == INT_TYPE) {
      expr_node->dataType = INT_TYPE;
      FreeReg(rs1, INT_T);
      FreeReg(rs2, INT_T);
      int rd = GetReg(INT_T);
      switch (semanticValue->op.binaryOp) {
        case BINARY_OP_ADD: {
          fprintf(fp, "\tadd\tx%d,x%d,x%d\n", rd, rs1, rs2);
          break;
        }
        case BINARY_OP_SUB: {
          fprintf(fp, "\tsub\tx%d,x%d,x%d\n", rd, rs1, rs2);
          break;
        }
        case BINARY_OP_MUL: {
          fprintf(fp, "\tmul\tx%d,x%d,x%d\n", rd, rs1, rs2);
          break;
        }
        case BINARY_OP_DIV: {
          fprintf(fp, "\tdiv\tx%d,x%d,x%d\n", rd, rs1, rs2);
          break;
        }
        case BINARY_OP_EQ: {
          fprintf(fp, "\tsub\tx%d,x%d,x%d\n", rd, rs1, rs2);
          fprintf(fp, "\tseqz\tx%d,x%d\n", rd, rd);
          break;
        }
        case BINARY_OP_NE: {
          fprintf(fp, "\tsub\tx%d,x%d,x%d\n", rd, rs1, rs2);
          fprintf(fp, "\tsnez\tx%d,x%d\n", rd, rd);
          break;
        }
        case BINARY_OP_GE: {
          fprintf(fp, "\tslt\tx%d,x%d,x%d\n", rd, rs1, rs2);
          fprintf(fp, "\txori\tx%d,x%d,1\n", rd, rd);
          break;
        }
        case BINARY_OP_GT: {
          fprintf(fp, "\tsgt\tx%d,x%d,x%d\n", rd, rs1, rs2);
          break;
        }
        case BINARY_OP_LE: {
          fprintf(fp, "\tsgt\tx%d,x%d,x%d\n", rd, rs1, rs2);
          fprintf(fp, "\txori\tx%d,x%d,1\n", rd, rd);
          break;
        }
        case BINARY_OP_LT: {
          fprintf(fp, "\tsgt\tx%d,x%d,x%d\n", rd, rs2, rs1);
          break;
        }
        case BINARY_OP_AND: {
          fprintf(fp, "\tbeqz\tx%d, .L%d\n", rs1, normal_label);
          fprintf(fp, "\tbeqz\tx%d, .L%d\n", rs2, normal_label);
          fprintf(fp, "\tli\tx%d, 1\n", rd);
          fprintf(fp, "\tj\t.L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli\tx%d, 0\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          break;
        }
        case BINARY_OP_OR: {
          fprintf(fp, "\tbnez\tx%d, .L%d\n", rs1, normal_label);
          fprintf(fp, "\tbnez\tx%d, .L%d\n", rs2, normal_label);
          fprintf(fp, "\tli\tx%d, 0\n", rd);
          fprintf(fp, "\tj\t.L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli\tx%d, 1\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          break;
        }
        default: {
          break;
        }
      }
      return rd;
    } else if (expr_node->child->dataType == FLOAT_TYPE && expr_node->child->rightSibling->dataType == FLOAT_TYPE) {
      FreeReg(rs1, FLOAT_T);
      FreeReg(rs2, FLOAT_T);
      int rd;
      switch (semanticValue->op.binaryOp) {
        case BINARY_OP_ADD: {
          rd = GetReg(FLOAT_T);
          fprintf(fp, "\tfadd.s\tf%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_SUB: {
          rd = GetReg(FLOAT_T);
          fprintf(fp, "\tfsub.s\tf%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_MUL: {
          rd = GetReg(FLOAT_T);
          fprintf(fp, "\tfmul.s\tf%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_DIV: {
          rd = GetReg(FLOAT_T);
          fprintf(fp, "\tfdiv.s\tf%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_EQ: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tfeq.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_NE: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tfeq.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          fprintf(fp, "\tseqz\tx%d,x%d\n", rd, rd);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_GE: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tfge.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_GT: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tfgt.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_LE: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tfle.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_LT: {
          rd = GetReg(INT_T);
          fprintf(fp, "\tflt.s\tx%d,f%d,f%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_AND: {
          rd = GetReg(INT_T);
          int zero = GetReg(FLOAT_T);
          int tmp = GetReg(INT_T);
          fprintf(fp, "\tfmv.s.x\tf%d, zero\n", zero);
          fprintf(fp, "\tfeq.s\tx%d, f%d, f%d\n", tmp, rs1, zero);
          fprintf(fp, "\tbnez\tx%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tfeq.s\tx%d, f%d, f%d\n", tmp, rs2, zero);
          fprintf(fp, "\tbnez\tx%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tli\tx%d, 1\n", rd);
          fprintf(fp, "\tj\t.L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli\tx%d, 0\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          FreeReg(tmp, INT_T);
          FreeReg(zero, FLOAT_T);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_OR: {
          rd = GetReg(INT_T);
          int zero = GetReg(FLOAT_T);
          int tmp = GetReg(INT_T);
          fprintf(fp, "\tfmv.s.x f%d, zero\n", zero);
          fprintf(fp, "\tfeq.s x%d, f%d, f%d\n", tmp, rs1, zero);
          fprintf(fp, "\tbeqz x%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tfeq.s x%d, f%d, f%d\n", tmp, rs2, zero);
          fprintf(fp, "\tbeqz x%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tli x%d, 0\n", rd);
          fprintf(fp, "\tj .L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli x%d, 1\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          FreeReg(tmp, INT_T);
          FreeReg(zero, FLOAT_T);
          expr_node->dataType = INT_TYPE;
          break;
        }
        default: {
          break;
        }
      }
      return rd;
    } else {
      // TODO: int <op> float or float <op> int
    }
  } else {  // unary operation
    int rs1 = GenExpr(expr_node->child);
    if (expr_node->child->dataType == INT_TYPE) {
      switch (semanticValue->op.unaryOp) {
        case UNARY_OP_LOGICAL_NEGATION: {
          fprintf(fp, "\tseqz\tx%d,x%d\n", rs1, rs1);
          fprintf(fp, "\txori\tx%d,x%d,1\n", rs1, rs1);
          break;
        }
        case UNARY_OP_NEGATIVE: {
          fprintf(fp, "\tsub\tx%d,x0,x%d\n", rs1, rs1);
          break;
        }
        case UNARY_OP_POSITIVE: {
          // do nothing
          break;
        }
      }
      expr_node->dataType = INT_TYPE;
      return rs1;
    } else {  // float type
      int rd;
      switch (semanticValue->op.unaryOp) {
        case UNARY_OP_LOGICAL_NEGATION: {
          int zero = GetReg(FLOAT_T);
          rd = GetReg(INT_T);
          fprintf(fp, "\tfmv.s.x f%d, zero\n", zero);
          fprintf(fp, "\tfeq.s x%d, f%d, f%d\n", rd, rs1, zero);
          fprintf(fp, "\txori\tx%d,x%d,1\n", rd, rd);
          FreeReg(zero, FLOAT_T);
          FreeReg(rs1, FLOAT_T);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case UNARY_OP_NEGATIVE: {
          int zero = GetReg(FLOAT_T);
          rd = GetReg(FLOAT_T);
          fprintf(fp, "\tfmv.s.x f%d, zero\n", zero);
          fprintf(fp, "\tfsub.s\tf%d,f%d,f%d\n", rd, zero, rs1);
          FreeReg(zero, FLOAT_T);
          FreeReg(rs1, FLOAT_T);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case UNARY_OP_POSITIVE: {
          // do nothing
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
      }
      return rd;
    }
  }
}

void PassParameter() {
}

void GenFunctionCall(AST_NODE *stmt_node) {
  AST_NODE *function_id_node = stmt_node->child;
  if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "write") == 0) {
    AST_NODE *parameter_node = function_id_node->rightSibling->child;
    if (parameter_node->dataType == CONST_STRING_TYPE) {
      char *s = (char *)malloc(strlen(parameter_node->semantic_value.const1->const_u.sc) - 2);
      strncpy(s, parameter_node->semantic_value.const1->const_u.sc + 1, strlen(parameter_node->semantic_value.const1->const_u.sc) - 2);
      fprintf(fp, ".data\n");
      fprintf(fp, ".SC%d: .string \"%s\\000\"\n", sc_label_number, s);
      fprintf(fp, ".text\n");
      int rs = GetReg(INT_T);
      fprintf(fp, "\tlui\tx%d,%%hi(.SC%d)\n", rs, sc_label_number);
      fprintf(fp, "\taddi\ta0,x%d,%%lo(.SC%d)\n", rs, sc_label_number);
      sc_label_number++;
      fprintf(fp, "\tcall\t_write_str\n");
      FreeReg(rs, INT_T);
    } else {
      int rs = GenExpr(parameter_node);
      assert(parameter_node->dataType != NONE_TYPE);
      if (parameter_node->dataType == INT_TYPE) {
        fprintf(fp, "\tmv\ta0,x%d\n", rs);
        fprintf(fp, "\tjal\t_write_int\n");
        FreeReg(rs, INT_T);
      } else if (parameter_node->dataType == FLOAT_TYPE) {
        fprintf(fp, "\tfmv.s\tfa0,f%d\n", rs);
        fprintf(fp, "\tjal\t_write_float\n");
        FreeReg(rs, FLOAT_T);
      }
    }
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "read") == 0) {
    fprintf(fp, "\tcall\t_read_int\n");
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "fread") == 0) {
    fprintf(fp, "\tcall\t_read_float\n");
  } else {
    StoreStaticVariables();
    // push parameter
    // push space for parameter
    fprintf(fp, "\tjal\t_start_%s\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
    // pop space for parameter
  }
}

void GenReturnStmt(AST_NODE *return_node) {
  AST_NODE *function_decl_node = return_node;
  while (function_decl_node->nodeType != DECLARATION_NODE && function_decl_node->semantic_value.declSemanticValue.kind != FUNCTION_DECL) {
    function_decl_node = function_decl_node->parent;
  }
  if (return_node->child) {  // with return value
    int rs = GenExpr(return_node->child);
    if (return_node->child->dataType == INT_TYPE) {
      fprintf(fp, "\tmv\ta0,x%d\n", rs);
    } else if (return_node->child->dataType == FLOAT_TYPE) {
      // TODO
      fprintf(fp, "\tfmv.s\tfa0,f%d\n", rs);
    }
    assert(return_node->child->dataType == INT_TYPE || return_node->child->dataType == FLOAT_TYPE);
    if (return_node->child->dataType == INT_TYPE) {
      FreeReg(rs, INT_T);
    } else {
      FreeReg(rs, FLOAT_T);
    }
  }
  StoreStaticVariables();
  fprintf(fp, "\tj\t_end_%s\n", function_decl_node->child->rightSibling->semantic_value.identifierSemanticValue.identifierName);
}

void GenIfStmt(AST_NODE *stmt_node) {
  AST_NODE *test_node = stmt_node->child;
  int test_reg = GenExpr(test_node);
  assert(test_node->dataType == INT_TYPE || test_node->dataType == FLOAT_TYPE);
  if (test_node->dataType == INT_TYPE) {
    FreeReg(test_reg, INT_T);
  } else {
    FreeReg(test_reg, FLOAT_T);
  }
  int label_number = max_label_number++;
  if (test_node->rightSibling->rightSibling->nodeType != NUL_NODE) {  // if-else statement
    fprintf(fp, "\tbeqz\tx%d,_Lelse%d\n", test_reg, label_number);
    GenStatement(test_node->rightSibling);
    fprintf(fp, "\tj\t_Lexit%d\n", label_number);
    fprintf(fp, "_Lelse%d:\n", label_number);
    GenStatement(test_node->rightSibling->rightSibling);
  } else {  // if-only statement
    fprintf(fp, "\tbeqz\tx%d,_Lexit%d\n", test_reg, label_number);
    GenStatement(test_node->rightSibling);
  }
  fprintf(fp, "_Lexit%d:\n", label_number);
}

void GenWhileStmt(AST_NODE *stmt_node) {
  AST_NODE *test_node = stmt_node->child;
  int label_number = max_label_number++;
  fprintf(fp, "_Test%d:\n", label_number);
  int test_reg = GenExpr(test_node);
  fprintf(fp, "\tbeqz\tx%d,_Lexit%d\n", test_reg, label_number);
  assert(test_node->dataType == INT_TYPE || test_node->dataType == FLOAT_TYPE);
  if (test_node->dataType == INT_TYPE) {
    FreeReg(test_reg, INT_T);
  } else {
    FreeReg(test_reg, FLOAT_T);
  }
  GenStatement(test_node->rightSibling);
  StoreDirtyRegisters();
  fprintf(fp, "\tj\t_Test%d\n", label_number);
  fprintf(fp, "_Lexit%d:\n", label_number);
}

void GenAssignment(AST_NODE *assignment_node) {
  AST_NODE *id_node = assignment_node->child;
  int rs = GenExpr(id_node->rightSibling);
  assert(id_node->rightSibling->dataType != NONE_TYPE);
  if (id_node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
    // TODO: type casting
    int gg = id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->reg;
    if (id_node->rightSibling->dataType == INT_TYPE) {
      int tmp_reg = (gg == 0) ? GetReg(INT_S) : gg;
      fprintf(fp, "\tmv\tx%d,x%d\n", tmp_reg, rs);
      FreeReg(rs, INT_T);
      reg_int[tmp_reg].entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
      reg_int[tmp_reg].entry->attribute->attr.typeDescriptor->reg = tmp_reg;
      reg_int[tmp_reg].dirty = 1;
      reg_int[tmp_reg].ref_count += 1;
    } else if (id_node->rightSibling->dataType == FLOAT_TYPE) {
      int tmp_reg = (gg == 0) ? GetReg(FLOAT_S) : gg;
      fprintf(fp, "\tmv\tf%d,f%d\n", tmp_reg, rs);
      FreeReg(rs, FLOAT_T);
      reg_float[tmp_reg].entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
      reg_float[tmp_reg].entry->attribute->attr.typeDescriptor->reg = tmp_reg;
      reg_float[tmp_reg].dirty = 1;
      reg_float[tmp_reg].ref_count += 1;
    }
  } else {  // array
    if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
      int tmp_reg = GetReg(INT_T);
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      int vp = GenVp(id_node);
      fprintf(fp, "\tadd\tx%d,x%d,x%d\n", tmp_reg, vp, tmp_reg);
      if (id_node->rightSibling->dataType == INT_TYPE) {
        fprintf(fp, "\tsw\tx%d,0(x%d)\n", rs, tmp_reg);
        FreeReg(rs, INT_T);
      } else if (id_node->rightSibling->dataType == FLOAT_TYPE) {
        fprintf(fp, "\tfsw\tf%d,0(x%d)\n", rs, tmp_reg);
        FreeReg(rs, FLOAT_T);
      }
      FreeReg(tmp_reg, INT_T);
      FreeReg(vp, INT_T);
    } else {
      int vp = GenVp(id_node);
      fprintf(fp, "\tadd\tx%d,fp,x%d\n", vp, vp);
      if (id_node->dataType == INT_TYPE) {
        fprintf(fp, "\tsw\tx%d,-%d(x%d)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset, vp);
        FreeReg(rs, INT_T);
      } else if (id_node->dataType == FLOAT_TYPE) {
        fprintf(fp, "\tfsw\tf%d,-%d(x%d)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset, vp);
        FreeReg(rs, FLOAT_T);
      }
      FreeReg(vp, INT_T);
    }
  }
}

void GenFunctionDeclaration(AST_NODE *declaration_node) {
  assert(declaration_node->semantic_value.declSemanticValue.kind == FUNCTION_DECL);
  AST_NODE *type_node = declaration_node->child;
  AST_NODE *function_id_node = type_node->rightSibling;
  fprintf(fp, "_start_%s:\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
  GenPrologue(function_id_node->semantic_value.identifierSemanticValue.identifierName);
  GenBlockNode(function_id_node->rightSibling->rightSibling);
  GenEpilogue(function_id_node->semantic_value.identifierSemanticValue.identifierName);
}

void CodeGen(AST_NODE *root, FILE *_fp) {
  fp = _fp;
  AST_NODE *node = root->child;
  InitRegs();
  while (node) {
    fflush(fp);
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      fprintf(fp, ".data\n");
      GenSymbolDeclaration(node);
    } else if (node->nodeType == DECLARATION_NODE) {
      fprintf(fp, ".text\n");
      GenFunctionDeclaration(node);
    }
    node = node->rightSibling;
  }
}
