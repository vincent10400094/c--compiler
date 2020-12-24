#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"

int AR_offset = 4;  //5-7, 18-31,

int int_reg[32];
int float_reg[8];
void GenPrologue(char *function_name, FILE *fp);
void GenEpilogue(char *function_name, FILE *fp);
int GetReg();
void FreeReg(int reg_number);
int GetFloatReg();
void FreeFloatReg(int reg_number);
int GetOffset();
void ResetOffset();
void AllocateSymbol(SymbolTableEntry *entry, int size);
void GenSymbolDeclaration(AST_NODE *declaration_list_node, FILE *fp);
void GenBlockNode(AST_NODE *block_node, FILE *fp);
void GenStatement(AST_NODE *statement_list_node, FILE *fp);
void GenFunctionDeclaration(AST_NODE *declaration_node, FILE *fp);
void GenSymbolReference();
int GenExpr(AST_NODE *expr_node, FILE *fp);
void GenAssignment(AST_NODE *assignment_node, FILE *fp);
void GenFunctionCall(AST_NODE *stmt_node, FILE *fp);
void symbolTableAdd(char *symbol_name);
void GenHead(AST_NODE *id_name_node);
void CodeGen(AST_NODE *root, FILE *fp);
void DoubleToHex(void *v, FILE *fp);
int LoadVariable(AST_NODE *id_node, FILE *fp);

int GetReg() {
  for (int i = 5; i <= 7; i++) {
    if (int_reg[i] == 0) {
      int_reg[i] = 1;
      return i;
    }
  }
  for (int i = 18; i <= 31; i++) {
    if (int_reg[i] == 0) {
      int_reg[i] = 1;
      return i;
    }
  }
  return -1;
}

void FreeReg(int reg_number) {
  int_reg[reg_number] = 0;
}

int GetFloatReg() {
  for (int i = 0; i < 8; i++) {
    if (float_reg[i] == 0) {
      float_reg[i] = 1;
      return i;
    }
  }
  return -1;
}

void FreeFloatReg(int reg_number) {
  float_reg[reg_number] = 0;
}

int GetOffset() {
  return AR_offset;
}

void ResetOffset() {
  AR_offset = 4;
}

void AllocateSymbol(SymbolTableEntry *entry, int size) {
  assert(entry != NULL);
  entry->attribute->attr.typeDescriptor->offset = GetOffset();
  AR_offset += size;
}

void GenSymbolDeclaration(AST_NODE *declaration_list_node, FILE *fp) {
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
                fprintf(fp, "_g_%s: .word\n", id_node->semantic_value.identifierSemanticValue.identifierName);
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
                  fprintf(fp, "_g_%s: .word %d\n", id_node->semantic_value.identifierSemanticValue.identifierName, float_to_int);
                }
              } else {
                //reg = GenExpr()
                //sw reg, -offset(fp)
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

void GenPrologue(char *function_name, FILE *fp) {
  fprintf(fp,
          ".text\n\tsd ra,0(sp)\n\tsd fp,-8(sp)\n\tadd fp,sp,-8\n\tadd sp,sp,-16\n\tla ra,_frameSize_%s\n\tlw ra,0(ra)\n\t\
sub sp,sp,ra\n\tsd t0,8(sp)\n\tsd t1,16(sp)\n\tsd t2,24(sp)\n\tsd t3,32(sp)\n\tsd t4,40(sp)\n\tsd t5,48(sp)\n\t\
sd t6,56(sp)\n\tsd s2,64(sp)\n\tsd s3,72(sp)\n\tsd s4,80(sp)\n\tsd s5,88(sp)\n\tsd s6,96(sp)\n\tsd s7,104(sp)\n\t\
sd s8,112(sp)\n\tsd s9,120(sp)\n\tsd s10,128(sp)\n\tsd s11,136(sp)\n\tsd fp,144(sp)\n\t\
fsw ft0,152(sp)\n\tfsw ft1,156(sp)\n\tfsw ft2,160(sp)\n\tfsw ft3,164(sp)\n\tfsw ft4,168(sp)\n\tfsw ft5,172(sp)\n\t\
fsw ft6,176(sp)\n\tfsw ft7,180(sp)\n",
          function_name);
}

void GenEpilogue(char *function_name, FILE *fp) {
  fprintf(fp,
          ".text\n\tld t0,8(sp)\n\tld t1,16(sp)\n\tld t2,24(sp)\n\tld t3,32(sp)\n\tld t4,40(sp)\n\tld t5,48(sp)\n\t\
ld t6,56(sp)\n\tld s2,64(sp)\n\tld s3,72(sp)\n\tld s4,80(sp)\n\tld s5,88(sp)\n\tld s6,96(sp)\n\tld s7,104(sp)\n\t\
ld s8,112(sp)\n\tld s9,120(sp)\n\tld s10,128(sp)\n\tld s11,136(sp)\n\tld fp,144(sp)\n\t\
flw ft0,152(sp)\n\tflw ft1,156(sp)\n\tflw ft2,160(sp)\n\tflw ft3,164(sp)\n\tflw ft4,168(sp)\n\tflw ft5,172(sp)\n\t\
flw ft6,176(sp)\n\tflw ft7,180(sp)\n\tld ra,8(fp)\n\tmv sp,fp\n\tadd sp,sp,8\n\tld fp,0(fp)\n\tjr ra\n.data\n");
  fprintf(fp, "_frameSize_%s: .word %d\n", function_name, 180 + AR_offset);
  AR_offset = 4;
}

void GenBlockNode(AST_NODE *block_node, FILE *fp) {
  AST_NODE *node = block_node->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      GenSymbolDeclaration(node, fp);
    } else if (node->nodeType == STMT_LIST_NODE) {
      GenStatement(node, fp);
    }
    node = node->rightSibling;
  }
}

void GenStatement(AST_NODE *statement_list_node, FILE *fp) {
  AST_NODE *stmt_node = statement_list_node->child;
  while (stmt_node) {
    if (stmt_node->nodeType == STMT_NODE) {
      switch (stmt_node->semantic_value.stmtSemanticValue.kind) {
        case ASSIGN_STMT: {
          GenAssignment(stmt_node, fp);
          break;
        }
        case IF_STMT: {
          break;
        }
        case FOR_STMT: {
          break;
        }
        case FUNCTION_CALL_STMT: {
          GenFunctionCall(stmt_node, fp);
          break;
        }
        case WHILE_STMT: {
          break;
        }
        case RETURN_STMT: {
          break;
        }
      }
    } else if (stmt_node->nodeType == BLOCK_NODE) {
      pushTable();
      GenBlockNode(stmt_node, fp);
      popTable();
    }
    stmt_node = stmt_node->rightSibling;
  }
}

// return value: register number that holds the value
int LoadVariable(AST_NODE *id_node, FILE *fp) {
  SymbolTableEntry *entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
  int tmp_reg, reg;
  if (entry->scope == 0) {  // static variable
    tmp_reg = GetReg();
    fprintf(fp, "\tla x%d, _%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
    reg = GetReg();
    fprintf(fp, "\tlw x%d, 0(%d)\n", reg, tmp_reg);
    FreeReg(tmp_reg);
  } else {  // local variable
    reg = GetReg();
    fprintf(fp, "\tlw x%d, -%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
  }
  return reg;
}

// return value: register number that holds the value
int GenExpr(AST_NODE *expr_node, FILE *fp) {
  if (expr_node->nodeType == CONST_VALUE_NODE) {
    if (expr_node->semantic_value.const1->const_type == INTEGERC) {
      int reg = GetReg();
      fprintf(fp, "\tli x%d, %d\n", reg, expr_node->semantic_value.const1->const_u.intval);
      return reg;
    } else if (expr_node->semantic_value.const1->const_type == FLOATC) {
    }
  } else if (expr_node->nodeType == IDENTIFIER_NODE) {
    return LoadVariable(expr_node, fp);
  } else if (expr_node->nodeType == STMT_NODE) {
    // gen function call
  }
  EXPRSemanticValue *semanticValue = &(expr_node->semantic_value.exprSemanticValue);
  if (semanticValue->kind == BINARY_OPERATION) {  // binary operation
    int rs1 = GenExpr(expr_node->child, fp);
    int rs2 = GenExpr(expr_node->child->rightSibling, fp);
    int rd = GetReg();
    switch (semanticValue->op.binaryOp) {
      case BINARY_OP_ADD: {
        fprintf(fp, "\tadd x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_SUB: {
        fprintf(fp, "\tsub x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_MUL: {
        fprintf(fp, "\tmul x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_DIV: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_EQ: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_NE: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_GE: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_GT: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_LE: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_LT: {
        fprintf(fp, "\tdiv x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_AND: {
        fprintf(fp, "\tand x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_OR: {
        fprintf(fp, "\tor x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      default: {
        break;
      }
    }
    FreeReg(rs1);
    FreeReg(rs2);
    return rd;
  } else {  // unary operation
    int rs1 = GenExpr(expr_node->child, fp);
    switch (semanticValue->op.unaryOp) {
      case UNARY_OP_LOGICAL_NEGATION: {
        break;
      }
      case UNARY_OP_NEGATIVE: {
        break;
      }
      case UNARY_OP_POSITIVE: {
        break;
      }
    }
    return rs1;
  }
}

void GenFunctionCall(AST_NODE *stmt_node, FILE *fp) {
  AST_NODE *function_id_node = stmt_node->child;
  if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "write") == 0) {
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "read") == 0) {
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "fread") == 0) {
  } else {
  }
}

void GenAssignment(AST_NODE *assignment_node, FILE *fp) {
  int rs = GenExpr(assignment_node->child->rightSibling, fp);
  fprintf(fp, "\tsw x%d, -%d(fp)\n", rs, assignment_node->child->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
  FreeReg(rs);
}

void GenFunctionDeclaration(AST_NODE *declaration_node, FILE *fp) {
  assert(declaration_node->semantic_value.declSemanticValue.kind == FUNCTION_DECL);
  AST_NODE *type_node = declaration_node->child;
  AST_NODE *function_id_node = type_node->rightSibling;
  fprintf(fp, ".data\n");
  fprintf(fp, "%s:\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
  GenPrologue(function_id_node->semantic_value.identifierSemanticValue.identifierName, fp);
  pushTable();
  GenBlockNode(function_id_node->rightSibling->rightSibling, fp);
  popTable();
  GenEpilogue(function_id_node->semantic_value.identifierSemanticValue.identifierName, fp);
}

void CodeGen(AST_NODE *root, FILE *fp) {
  AST_NODE *node = root->child;
  while (node) {
    fflush(fp);
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      GenSymbolDeclaration(node, fp);
    } else if (node->nodeType == DECLARATION_NODE) {
      GenFunctionDeclaration(node, fp);
    }
    node = node->rightSibling;
  }
}