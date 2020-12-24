#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"

int AR_offset = 4;  //5-7, 18-31,
int max_label_number = 1;
int sc_label_number = 1;
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
                //reg = GenExpr()
                //sw\treg, -offset(fp)
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
      fprintf(fp, ".data\n");
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
    // pushTable();
    GenBlockNode(stmt_node, fp);
    // popTable();
  }
}

// return value: register number that holds the value
int LoadVariable(AST_NODE *id_node, FILE *fp) {
  SymbolTableEntry *entry = id_node->semantic_value.identifierSemanticValue.symbolTableEntry;
  int tmp_reg, reg;
  if (entry->scope == 0) {  // static variable
    tmp_reg = GetReg();
    fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
    reg = GetReg();
    fprintf(fp, "\tlw\tx%d,0(x%d)\n", reg, tmp_reg);
    FreeReg(tmp_reg);
  } else {  // local variable
    reg = GetReg();
    fprintf(fp, "\tlw\tx%d,-%d(fp)\n", reg, entry->attribute->attr.typeDescriptor->offset);
  }
  return reg;
}

// return value: register number that holds the value
int GenExpr(AST_NODE *expr_node, FILE *fp) {
  if (expr_node->nodeType == CONST_VALUE_NODE) {
    if (expr_node->semantic_value.const1->const_type == INTEGERC) {
      int reg = GetReg();
      fprintf(fp, "\tli\tx%d,%d\n", reg, expr_node->semantic_value.const1->const_u.intval);
      return reg;
    } else if (expr_node->semantic_value.const1->const_type == FLOATC) {
    }
  } else if (expr_node->nodeType == IDENTIFIER_NODE) {
    return LoadVariable(expr_node, fp);
  } else if (expr_node->nodeType == STMT_NODE) {
    // gen function call
    if ((expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry && expr_node->child->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.functionSignature->returnType == INT_TYPE) ||
        !strcmp(expr_node->child->semantic_value.identifierSemanticValue.identifierName, "read")) {
      //   fprintf(stderr, "hihi\n");
      GenFunctionCall(expr_node, fp);
      int reg = GetReg();
      fprintf(fp, "\tmv x%d, a0\n", reg);
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
        fprintf(fp, "\tsub x%d, x%d, x%d\n", rd, rs1, rs2);
        fprintf(fp, "\tseqz x%d, x%d\n", rd, rd);
        break;
      }
      case BINARY_OP_NE: {
        break;
      }
      case BINARY_OP_GE: {
        break;
      }
      case BINARY_OP_GT: {
        fprintf(fp, "\tsgt x%d, x%d, x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_LE: {
        break;
      }
      case BINARY_OP_LT: {
        break;
      }
      case BINARY_OP_AND: {
        fprintf(fp, "\tand\tx%d,x%d,x%d\n", rd, rs1, rs2);
        break;
      }
      case BINARY_OP_OR: {
        fprintf(fp, "\tor\tx%d,x%d,x%d\n", rd, rs1, rs2);
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
        fprintf(fp, "\tsub\t%d,x0,x%d\n", rs1, rs1);
        break;
      }
      case UNARY_OP_POSITIVE: {
        break;
      }
    }
    return rs1;
  }
}

void PassParameter() {
}

void GenFunctionCall(AST_NODE *stmt_node, FILE *fp) {
  AST_NODE *function_id_node = stmt_node->child;
  if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "write") == 0) {
    AST_NODE *parameter_node = function_id_node->rightSibling->child;
    if (parameter_node->dataType == CONST_STRING_TYPE) {
      char *s = (char *)malloc(strlen(parameter_node->semantic_value.const1->const_u.sc));
      strncpy(s, parameter_node->semantic_value.const1->const_u.sc + 1, strlen(parameter_node->semantic_value.const1->const_u.sc) - 2);
      fprintf(fp, ".data\n");
      fprintf(fp, ".SC%d: .string \"%s\\n\\000\"\n", sc_label_number, s);
      fprintf(fp, ".text\n");
      int rs = GetReg();
      fprintf(fp, "\tlui x%d, %%hi(.SC%d)\n", rs, sc_label_number);
      fprintf(fp, "\taddi a0, x%d, %%lo(.SC%d)\n", rs, sc_label_number);
      sc_label_number++;
      fprintf(fp, "\tcall _write_str\n");
      FreeReg(rs);
    } else {
      int rs = GenExpr(parameter_node, fp);
      if (parameter_node->dataType == INT_TYPE) {
        fprintf(fp, "\tmv a0, x%d\n", rs);
        fprintf(fp, "\tjal _write_int\n");
      } else {
        fprintf(fp, "\tfmv.s fa0, ft%d\n", rs);
        fprintf(fp, "\tjal _write_float\n");
      }
      FreeReg(rs);
    }
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "read") == 0) {
    fprintf(fp, "\tcall _read_int\n");
  } else if (strcmp(function_id_node->semantic_value.identifierSemanticValue.identifierName, "fread") == 0) {
    fprintf(fp, "\tcall _read_float\n");
  } else {
    // push parameter
    // push space for parameter
    fprintf(fp, "\tjal _start_%s\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
    // pop space for parameter
  }
}

void GenReturnStmt(AST_NODE *return_node, FILE *fp) {
  AST_NODE *function_decl_node = return_node;
  while (function_decl_node->nodeType != DECLARATION_NODE && function_decl_node->semantic_value.declSemanticValue.kind != FUNCTION_DECL) {
    function_decl_node = function_decl_node->parent;
  }
  int rs = GenExpr(return_node->child, fp);
  fprintf(fp, "\tmv a0, x%d\n", rs);
  fprintf(fp, "\tj _end_%s\n", function_decl_node->child->rightSibling->semantic_value.identifierSemanticValue.identifierName);
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
  if (id_node->semantic_value.identifierSemanticValue.symbolTableEntry->scope == 0) {
    int tmp_reg = GetReg();
    fprintf(fp, "\tla\tx%d,_g_%s\n", tmp_reg, id_node->semantic_value.identifierSemanticValue.identifierName);
    fprintf(fp, "\tsw\tx%d,0(x%d)\n", rs, tmp_reg);
    FreeReg(tmp_reg);
  } else {
    fprintf(fp, "\tsw\tx%d,-%d(fp)\n", rs, id_node->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor->offset);
  }
  FreeReg(rs);
}

void GenFunctionDeclaration(AST_NODE *declaration_node, FILE *fp) {
  assert(declaration_node->semantic_value.declSemanticValue.kind == FUNCTION_DECL);
  AST_NODE *type_node = declaration_node->child;
  AST_NODE *function_id_node = type_node->rightSibling;
  fprintf(fp, "_start_%s:\n", function_id_node->semantic_value.identifierSemanticValue.identifierName);
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
      fprintf(fp, ".data\n");
      GenSymbolDeclaration(node, fp);
    } else if (node->nodeType == DECLARATION_NODE) {
      fprintf(fp, ".text\n");
      GenFunctionDeclaration(node, fp);
    }
    node = node->rightSibling;
  }
}