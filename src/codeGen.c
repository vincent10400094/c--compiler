#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"

int AR_offset = 4;  //5-7, 18-31,
int max_label_number = 1;
int sc_label_number = 1;
int fconst_label_number = 1;
int normal_label = 1;
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
void GenReturnStmt(AST_NODE *return_node, FILE *fp);
void PassParameter();
void GenFunctionCall(AST_NODE *stmt_node, FILE *fp);
void GenIfStmt(AST_NODE *stmt_node, FILE *fp);
void GenWhileStmt(AST_NODE *stmt_node, FILE *fp);
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
                fprintf(fp, "_g_%s: .word 0\n", id_node->semantic_value.identifierSemanticValue.identifierName);
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
                AST_NODE *init_node = id_node->child;
                int rs = GenExpr(id_node->child, fp);
                assert(init_node->dataType != NONE_TYPE);
                if (init_node->dataType == INT_TYPE) {
                  AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
                  fprintf(fp, "\tsw\tx%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
                  FreeReg(rs);
                } else if (init_node->dataType == FLOAT_TYPE) {
                  AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
                  fprintf(fp, "\tfsw\tft%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
                  FreeFloatReg(rs);
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

void GenPrologue(char *function_name, FILE *fp) {
  fprintf(fp,
          "\tsd\tra,0(sp)\n\tsd\tfp,-8(sp)\n\tadd fp,sp,-8\n\tadd sp,sp,-16\n\tla\tra,_frameSize_%s\n\tlw\tra,0(ra)\n\t\
sub\tsp,sp,ra\n\tsd\tt0,8(sp)\n\tsd\tt1,16(sp)\n\tsd\tt2,24(sp)\n\tsd\tt3,32(sp)\n\tsd\tt4,40(sp)\n\tsd\tt5,48(sp)\n\t\
sd\tt6,56(sp)\n\tsd\ts2,64(sp)\n\tsd\ts3,72(sp)\n\tsd\ts4,80(sp)\n\tsd\ts5,88(sp)\n\tsd\ts6,96(sp)\n\tsd\ts7,104(sp)\n\t\
sd\ts8,112(sp)\n\tsd\ts9,120(sp)\n\tsd\ts10,128(sp)\n\tsd\ts11,136(sp)\n\tsd\tfp,144(sp)\n\t\
fsw\tft0,152(sp)\n\tfsw\tft1,156(sp)\n\tfsw\tft2,160(sp)\n\tfsw\tft3,164(sp)\n\tfsw\tft4,168(sp)\n\tfsw\tft5,172(sp)\n\t\
fsw\tft6,176(sp)\n\tfsw\tft7,180(sp)\n",
          function_name);
}

void GenEpilogue(char *function_name, FILE *fp) {
  fprintf(fp, "_end_%s:\n", function_name);
  fprintf(fp,
          "\tld\tt0,8(sp)\n\tld\tt1,16(sp)\n\tld\tt2,24(sp)\n\tld\tt3,32(sp)\n\tld\tt4,40(sp)\n\tld\tt5,48(sp)\n\t\
ld\tt6,56(sp)\n\tld\ts2,64(sp)\n\tld\ts3,72(sp)\n\tld\ts4,80(sp)\n\tld\ts5,88(sp)\n\tld\ts6,96(sp)\n\tld\ts7,104(sp)\n\t\
ld\ts8,112(sp)\n\tld\ts9,120(sp)\n\tld\ts10,128(sp)\n\tld\ts11,136(sp)\n\tld\tfp,144(sp)\n\t\
flw\tft0,152(sp)\n\tflw\tft1,156(sp)\n\tflw\tft2,160(sp)\n\tflw\tft3,164(sp)\n\tflw\tft4,168(sp)\n\tflw\tft5,172(sp)\n\t\
flw\tft6,176(sp)\n\tflw\tft7,180(sp)\n\tld\tra,8(fp)\n\tmv\tsp,fp\n\tadd sp,sp,8\n\tld\tfp,0(fp)\n\tjr\tra\n.data\n");
  fprintf(fp, "_frameSize_%s: .word %d\n", function_name, 180 + AR_offset);
  AR_offset = 4;
}

void GenBlockNode(AST_NODE *block_node, FILE *fp) {
  AST_NODE *node = block_node->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      GenSymbolDeclaration(node, fp);
      fprintf(fp, ".text\n");
    } else if (node->nodeType == STMT_LIST_NODE) {
      AST_NODE *stmt_node = node->child;
      while (stmt_node) {
        GenStatement(stmt_node, fp);
        stmt_node = stmt_node->rightSibling;
      }
    }
    node = node->rightSibling;
  }
}

void GenStatement(AST_NODE *stmt_node, FILE *fp) {
  if (stmt_node->nodeType == STMT_NODE) {
    switch (stmt_node->semantic_value.stmtSemanticValue.kind) {
      case ASSIGN_STMT: {
        GenAssignment(stmt_node, fp);
        break;
      }
      case IF_STMT: {
        GenIfStmt(stmt_node, fp);
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
        GenWhileStmt(stmt_node, fp);
        break;
      }
      case RETURN_STMT: {
        GenReturnStmt(stmt_node, fp);
        break;
      }
    }
  } else if (stmt_node->nodeType == BLOCK_NODE) {
    GenBlockNode(stmt_node, fp);
  }
}

// return value: register number that holds the value
int LoadVariable(AST_NODE *id_node, FILE *fp) {
  SymbolTableEntry *entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
  assert(id_node->dataType != NONE_TYPE);
  int tmp_reg, reg;
  if (entry->scope == 0) {  // static variable
    if (id_node->dataType == INT_TYPE) {
      tmp_reg = GetReg();
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      reg = GetReg();
      fprintf(fp, "\tlw\tx%d,0(x%d)\n", reg, tmp_reg);
      FreeReg(tmp_reg);
    } else if (id_node->dataType == FLOAT_TYPE) {
      tmp_reg = GetReg();
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      reg = GetFloatReg();
      fprintf(fp, "\tflw\tft%d,0(x%d)\n", reg, tmp_reg);
      FreeReg(tmp_reg);
    }
  } else {  // local variable
    if (id_node->dataType == INT_TYPE) {
      reg = GetReg();
      fprintf(fp, "\tlw\tx%d,-%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
    } else if (id_node->dataType == FLOAT_TYPE) {
      reg = GetFloatReg();
      fprintf(fp, "\tflw\tft%d,-%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
    }
  }
  return reg;
}

// return value: register number that holds the value
int GenExpr(AST_NODE *expr_node, FILE *fp) {
  if (expr_node->nodeType == CONST_VALUE_NODE) {
    if (expr_node->semantic_value.const1->const_type == INTEGERC) {
      expr_node->dataType = INT_TYPE;
      int reg = GetReg();
      fprintf(fp, "\tli\tx%d,%d\n", reg, expr_node->semantic_value.const1->const_u.intval);
      return reg;
    } else if (expr_node->semantic_value.const1->const_type == FLOATC) {
      expr_node->dataType = FLOAT_TYPE;
      fprintf(fp, ".data\n");
      float fconst = expr_node->semantic_value.const1->const_u.fval;
      int float_to_int = *(int *)&fconst;
      fprintf(fp, ".FC%d: .word %u\n", fconst_label_number, float_to_int);
      fprintf(fp, ".text\n");
      int tmp_reg = GetReg();
      fprintf(fp, "\tla\tx%d, .FC%d\n", tmp_reg, fconst_label_number);
      int reg = GetFloatReg();
      fprintf(fp, "\tflw\tft%d,0(x%d)\n", reg, tmp_reg);
      FreeReg(tmp_reg);
      fconst_label_number++;
      return reg;
    }
  } else if (expr_node->nodeType == IDENTIFIER_NODE) {
    expr_node->dataType = expr_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->properties.dataType;
    return LoadVariable(expr_node, fp);
  } else if (expr_node->nodeType == STMT_NODE) {
    // gen function call
    if ((expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry && expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.functionSignature->returnType == INT_TYPE) ||
        !strcmp(expr_node->child->semantic_value.identifierSemanticValue.identifierName, "read")) {
      GenFunctionCall(expr_node, fp);
      int reg = GetReg();
      fprintf(fp, "\tmv\tx%d,a0\n", reg);
      expr_node->dataType = INT_TYPE;
      return reg;
    } else {
      GenFunctionCall(expr_node, fp);
      int reg = GetFloatReg();
      fprintf(fp, "\tfmv.s ft%d, fa0\n", reg);
      expr_node->dataType = FLOAT_TYPE;
      return reg;
    }
  }
  EXPRSemanticValue *semanticValue = &(expr_node->semantic_value.exprSemanticValue);
  if (semanticValue->kind == BINARY_OPERATION) {  // binary operation
    int rs1 = GenExpr(expr_node->child, fp);
    int rs2 = GenExpr(expr_node->child->rightSibling, fp);
    assert(expr_node->child->dataType != NONE_TYPE);
    char reg_name[3];
    if (expr_node->child->dataType == INT_TYPE) {
      FreeReg(rs1);
      FreeReg(rs2);
      int rd = GetReg();
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
      expr_node->dataType = INT_TYPE;
      return rd;
    } else {
      FreeFloatReg(rs1);
      FreeFloatReg(rs2);
      int rd;
      switch (semanticValue->op.binaryOp) {
        case BINARY_OP_ADD: {
          rd = GetFloatReg();
          fprintf(fp, "\tfadd.s\tft%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_SUB: {
          rd = GetFloatReg();
          fprintf(fp, "\tfsub.s\tft%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_MUL: {
          rd = GetFloatReg();
          fprintf(fp, "\tfmul.s\tft%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_DIV: {
          rd = GetFloatReg();
          fprintf(fp, "\tfdiv.s\tft%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = FLOAT_TYPE;
          break;
        }
        case BINARY_OP_EQ: {
          rd = GetReg();
          fprintf(fp, "\tfeq.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_NE: {
          rd = GetReg();
          fprintf(fp, "\tfeq.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          fprintf(fp, "\tseqz\tx%d,x%d\n", rd, rd);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_GE: {
          rd = GetReg();
          fprintf(fp, "\tfge.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_GT: {
          rd = GetReg();
          fprintf(fp, "\tfgt.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_LE: {
          rd = GetReg();
          fprintf(fp, "\tfle.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_LT: {
          rd = GetReg();
          fprintf(fp, "\tflt.s\tx%d,ft%d,ft%d\n", rd, rs1, rs2);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case BINARY_OP_AND: {
          rd = GetReg();
          int zero = GetFloatReg();
          int tmp = GetReg();
          fprintf(fp, "\tfmv.s.x\tft%d, zero\n", zero);
          fprintf(fp, "\tfeq.s\tx%d, ft%d, ft%d\n", tmp, rs1, zero);
          fprintf(fp, "\tbnez\tx%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tfeq.s\tx%d, ft%d, ft%d\n", tmp, rs2, zero);
          fprintf(fp, "\tbnez\tx%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tli\tx%d, 1\n", rd);
          fprintf(fp, "\tj\t.L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli\tx%d, 0\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          FreeReg(tmp);
          FreeFloatReg(zero);
          break;
        }
        case BINARY_OP_OR: {
          rd = GetReg();
          int zero = GetFloatReg();
          int tmp = GetReg();
          fprintf(fp, "\tfmv.s.x ft%d, zero\n", zero);
          fprintf(fp, "\tfeq.s x%d, ft%d, ft%d\n", tmp, rs1, zero);
          fprintf(fp, "\tbeqz x%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tfeq.s x%d, ft%d, ft%d\n", tmp, rs2, zero);
          fprintf(fp, "\tbeqz x%d, .L%d\n", tmp, normal_label);
          fprintf(fp, "\tli x%d, 0\n", rd);
          fprintf(fp, "\tj .L%d\n", normal_label + 1);
          fprintf(fp, ".L%d:\n", normal_label);
          fprintf(fp, "\tli x%d, 1\n", rd);
          fprintf(fp, ".L%d:\n", normal_label + 1);
          normal_label += 2;
          FreeReg(tmp);
          FreeFloatReg(zero);
          break;
        }
        default: {
          break;
        }
      }
      return rd;
    }
  } else {  // unary operation
    int rs1 = GenExpr(expr_node->child, fp);
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
    } else {
      int rd;
      switch (semanticValue->op.unaryOp) {
        case UNARY_OP_LOGICAL_NEGATION: {
          int zero = GetFloatReg();
          rd = GetReg();
          fprintf(fp, "\tfmv.s.x ft%d, zero\n", zero);
          fprintf(fp, "\tfeq.s x%d, ft%d, ft%d\n", rd, rs1, zero);
          fprintf(fp, "\txori\tx%d,x%d,1\n", rd, rd);
          FreeFloatReg(zero);
          FreeFloatReg(rs1);
          expr_node->dataType = INT_TYPE;
          break;
        }
        case UNARY_OP_NEGATIVE: {
          int zero = GetFloatReg();
          rd = GetFloatReg();
          fprintf(fp, "\tfmv.s.x ft%d, zero\n", zero);
          fprintf(fp, "\tfsub.s\tft%d,ft%d,ft%d\n", rd, zero, rs1);
          FreeFloatReg(zero);
          FreeFloatReg(rs1);
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

void GenFunctionCall(AST_NODE *stmt_node, FILE *fp) {
  AST_NODE *function_id_node = stmt_node->child;
  if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "write") == 0) {
    AST_NODE *parameter_node = function_id_node->rightSibling->child;
    if (parameter_node->dataType == CONST_STRING_TYPE) {
      char *s = (char *)malloc(strlen(parameter_node->semantic_value.const1->const_u.sc) - 2);
      strncpy(s, parameter_node->semantic_value.const1->const_u.sc + 1, strlen(parameter_node->semantic_value.const1->const_u.sc) - 2);
      fprintf(fp, ".data\n");
      fprintf(fp, ".SC%d: .string \"%s\\000\"\n", sc_label_number, s);
      fprintf(fp, ".text\n");
      int rs = GetReg();
      fprintf(fp, "\tlui\tx%d,%%hi(.SC%d)\n", rs, sc_label_number);
      fprintf(fp, "\taddi\ta0,x%d,%%lo(.SC%d)\n", rs, sc_label_number);
      sc_label_number++;
      fprintf(fp, "\tcall\t_write_str\n");
      FreeReg(rs);
    } else {
      int rs = GenExpr(parameter_node, fp);
      if (parameter_node->dataType == INT_TYPE) {
        fprintf(fp, "\tmv\ta0,x%d\n", rs);
        fprintf(fp, "\tjal\t_write_int\n");
        FreeReg(rs);
      } else {
        fprintf(fp, "\tfmv.s fa0,ft%d\n", rs);
        fprintf(fp, "\tjal\t_write_float\n");
        FreeFloatReg(rs);
      }
    }
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "read") == 0) {
    fprintf(fp, "\tcall\t_read_int\n");
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "fread") == 0) {
    fprintf(fp, "\tcall\t_read_float\n");
  } else {
    // push parameter
    // push space for parameter
    fprintf(fp, "\tjal\t_start_%s\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
    // pop space for parameter
  }
}

void GenReturnStmt(AST_NODE *return_node, FILE *fp) {
  AST_NODE *function_decl_node = return_node;
  while (function_decl_node->nodeType != DECLARATION_NODE && function_decl_node->semantic_value.declSemanticValue.kind != FUNCTION_DECL) {
    function_decl_node = function_decl_node->parent;
  }
  int rs = GenExpr(return_node->child, fp);
  fprintf(fp, "\tmv\ta0,x%d\n", rs);
  fprintf(fp, "\tj\t_end_%s\n", function_decl_node->child->rightSibling->semantic_value.identifierSemanticValue.identifierName);
  FreeReg(rs);
}

void GenIfStmt(AST_NODE *stmt_node, FILE *fp) {
  AST_NODE *test_node = stmt_node->child;
  int test_reg = GenExpr(test_node, fp);
  FreeReg(test_reg);
  int label_number = max_label_number++;
  if (test_node->rightSibling->rightSibling->nodeType != NUL_NODE) {  // if-else statement
    fprintf(fp, "\tbeqz\tx%d,_Lelse%d\n", test_reg, label_number);
    GenStatement(test_node->rightSibling, fp);
    fprintf(fp, "\tj\t_Lexit%d\n", label_number);
    fprintf(fp, "_Lelse%d:\n", label_number);
    GenStatement(test_node->rightSibling->rightSibling, fp);
  } else {  // if-only statement
    fprintf(fp, "\tbeqz\tx%d,_Lexit%d\n", test_reg, label_number);
    GenStatement(test_node->rightSibling, fp);
  }
  fprintf(fp, "_Lexit%d:\n", label_number);
}

void GenWhileStmt(AST_NODE *stmt_node, FILE *fp) {
  AST_NODE *test_node = stmt_node->child;
  int label_number = max_label_number++;
  fprintf(fp, "_Test%d:\n", label_number);
  int test_reg = GenExpr(test_node, fp);
  FreeReg(test_reg);
  fprintf(fp, "\tbeqz\tx%d,_Lexit%d\n", test_reg, label_number);
  GenStatement(test_node->rightSibling, fp);
  fprintf(fp, "\tj\t_Test%d\n", label_number);
  fprintf(fp, "_Lexit%d:\n", label_number);
}

void GenAssignment(AST_NODE *assignment_node, FILE *fp) {
  AST_NODE *id_node = assignment_node->child;
  int rs = GenExpr(id_node->rightSibling, fp);
  assert(id_node->rightSibling->dataType != NONE_TYPE);
  if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
    if (id_node->rightSibling->dataType == INT_TYPE) {
      int tmp_reg = GetReg();
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      fprintf(fp, "\tsw\tx%d,0(x%d)\n", rs, tmp_reg);
      FreeReg(tmp_reg);
      FreeReg(rs);
    } else if (id_node->rightSibling->dataType == FLOAT_TYPE) {
      int tmp_reg = GetFloatReg();
      fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
      fprintf(fp, "\tfsw\tft%d,0(x%d)\n", rs, tmp_reg);
    }
  } else {
    if (id_node->rightSibling->dataType == INT_TYPE) {
      fprintf(fp, "\tsw\tx%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
      FreeReg(rs);
    } else if (id_node->rightSibling->dataType == FLOAT_TYPE) {
      fprintf(fp, "\tfsw\tft%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
      FreeFloatReg(rs);
    }
  }
}

void GenFunctionDeclaration(AST_NODE *declaration_node, FILE *fp) {
  assert(declaration_node->semantic_value.declSemanticValue.kind == FUNCTION_DECL);
  AST_NODE *type_node = declaration_node->child;
  AST_NODE *function_id_node = type_node->rightSibling;
  fprintf(fp, "_start_%s:\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
  GenPrologue(function_id_node->semantic_value.identifierSemanticValue.identifierName, fp);
  GenBlockNode(function_id_node->rightSibling->rightSibling, fp);
  GenEpilogue(function_id_node->semantic_value.identifierSemanticValue.identifierName, fp);
}

void CodeGen(AST_NODE *root, FILE *fp) {
  AST_NODE *node = root->child;
  while (node) {
    fflush(fp);
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      fprintf(fp, ".data\n");
      GenSymbolDeclaration(node, fp);
    } else if (node->nodeType == DECLARATION_NODE) {
      fprintf(fp, ".text\n");
      GenFunctionDeclaration(node, fp);
    }
    node = node->rightSibling;
  }
}
