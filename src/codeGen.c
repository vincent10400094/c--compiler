#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"

int AR_offset = 4;
int save_reg_number = 2;
int temp_reg_number = 0;

void GenPrologue(char *function_name, FILE *fp);
void GenEpilogue(char *function_name, FILE *fp);
int GetReg();
int GetOffset();
void ResetOffset();
void AllocateSymbol(SymbolTableEntry *entry, int size);
void GenSymbolDeclaration(AST_NODE *declaration_list_node, FILE *fp);
void GenBlockNode(AST_NODE *block_node, FILE *fp);
void GenStatement(AST_NODE *statement_list_node, FILE *fp);
void GenFunctionDeclaration(AST_NODE *declaration_node, FILE *fp);
void GenSymbolReference();
void GenExpr();
void GenAssignment(AST_NODE *assignment_node, FILE *fp);
void symbolTableAdd(char *symbol_name);
void GenHead(AST_NODE *id_name_node);
void CodeGen(AST_NODE *root, FILE *fp);
void DoubleToHex(void *v, FILE *fp);

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
              if (currentScope() == 0) {
                fprintf(fp, "_g_%s: .word\n", id_node->semantic_value.identifierSemanticValue.identifierName);
              } else {
                AllocateSymbol(id_node->semantic_value.identifierSemanticValue.symbolTableEntry, 4);
              }
            } else if (id_node->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID) {
              if (currentScope() == 0) {
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
            if (currentScope() == 0) {
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
          ".text\nsd ra,0(sp)\nsd fp,-8(sp)\nadd fp,sp,-8\nadd sp,sp,-16\nla ra,_frameSize_%s\nlw ra,0(ra)\n\
sub sp,sp,ra\nsd t0,8(sp)\nsd t1,16(sp)\nsd t2,24(sp)\nsd t3,32(sp)\nsd t4,40(sp)\nsd t5,48(sp)\n\
sd t6,56(sp)\nsd s2,64(sp)\nsd s3,72(sp)\nsd s4,80(sp)\nsd s5,88(sp)\nsd s6,96(sp)\nsd s7,104(sp)\n\
sd s8,112(sp)\nsd s9,120(sp)\nsd s10,128(sp)\nsd s11,136(sp)\nsd fp,144(sp)\n\
fsw ft0,152(sp)\nfsw ft1,156(sp)\nfsw ft2,160(sp)\nfsw ft3,164(sp)\nfsw ft4,168(sp)\nfsw ft5,172(sp)\n\
fsw ft6,176(sp)\nfsw ft7,180(sp)\n",
          function_name);
}

void GenEpilogue(char *function_name, FILE *fp) {
  fprintf(fp,
          ".text\nld t0,8(sp)\nld t1,16(sp)\nld t2,24(sp)\nld t3,32(sp)\nld t4,40(sp)\nld t5,48(sp)\n\
ld t6,56(sp)\nld s2,64(sp)\nld s3,72(sp)\nld s4,80(sp)\nld s5,88(sp)\nld s6,96(sp)\nld s7,104(sp)\n\
ld s8,112(sp)\nld s9,120(sp)\nld s10,128(sp)\nld s11,136(sp)\nld fp,144(sp)\n\
flw ft0,152(sp)\nflw ft1,156(sp)\nflw ft2,160(sp)\nflw ft3,164(sp)\nflw ft4,168(sp)\nflw ft5,172(sp)\n\
flw ft6,176(sp)\nflw ft7,180(sp)\nld ra,8(fp)\nmv sp,fp\nadd sp,sp,8\nld fp,0(fp)\njr ra\n.data\n");
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

void GenAssignment(AST_NODE *assignment_node, FILE *fp) {
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