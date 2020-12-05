#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "header.h"
#include "symbolTable.h"
// This file is for reference only, you are not required to follow the implementation. //
// You only need to check for errors stated in the hw4 document. //
int g_anyErrorOccur = 0;

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2);
void processProgramNode(AST_NODE* programNode);
void processDeclarationNode(AST_NODE* declarationNode);
void declareIdList(AST_NODE* typeNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize);
void variableDeclareList(AST_NODE* declarationNode);
void declareFunction(AST_NODE* returnTypeNode);
void processDeclDimList(AST_NODE* variableDeclDimList, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize);
void processTypeNode(AST_NODE* typeNode);
void processBlockNode(AST_NODE* blockNode);
void processStmtNode(AST_NODE* stmtNode);
void processGeneralNode(AST_NODE* node);
void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode);
void checkWhileStmt(AST_NODE* whileNode);
void checkForStmt(AST_NODE* forNode);
void checkAssignmentStmt(AST_NODE* assignmentNode);
void checkIfStmt(AST_NODE* ifNode);
void checkWriteFunction(AST_NODE* functionCallNode);
void checkFunctionCall(AST_NODE* functionCallNode);
void processExprRelatedNode(AST_NODE* exprRelatedNode);
void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter);
void checkReturnStmt(AST_NODE* returnNode);
void processExprNode(AST_NODE* exprNode);
// void processVariableLValue(AST_NODE* idNode);
// void processVariableRValue(AST_NODE* idNode);
void processVariableValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
DATA_TYPE getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);

typedef enum ErrorMsgKind {
  SYMBOL_IS_NOT_TYPE,
  SYMBOL_REDECLARE,
  SYMBOL_UNDECLARED,
  NOT_FUNCTION_NAME,
  TRY_TO_INIT_ARRAY,
  EXCESSIVE_ARRAY_DIM_DECLARATION,
  RETURN_ARRAY,
  VOID_VARIABLE,
  TYPEDEF_VOID_ARRAY,
  PARAMETER_TYPE_UNMATCH,
  TOO_FEW_ARGUMENTS,
  TOO_MANY_ARGUMENTS,
  RETURN_TYPE_UNMATCH,
  INCOMPATIBLE_ARRAY_DIMENSION,
  NOT_ASSIGNABLE,
  NOT_ARRAY,
  IS_TYPE_NOT_VARIABLE,
  IS_FUNCTION_NOT_VARIABLE,
  STRING_OPERATION,
  ARRAY_SIZE_NOT_INT,
  ARRAY_SIZE_NEGATIVE,
  ARRAY_SUBSCRIPT_NOT_INT,
  PASS_ARRAY_TO_SCALAR,
  PASS_SCALAR_TO_ARRAY
} ErrorMsgKind;

void printErrorMsgSpecial(AST_NODE* node1, char* name2, ErrorMsgKind errorMsgKind) {
  g_anyErrorOccur = 1;
  // printf("Error found in line %d\n", node1->linenumber);

  printf("%s:%d: ", srcPath, node1->linenumber);
  printf(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET);

  switch (errorMsgKind) {
    case SYMBOL_REDECLARE: {
      printf("redefinition of '%s'\n", name2);
      break;
    }
    case SYMBOL_UNDECLARED: {
      printf("use of undeclared identifier '%s'\n", name2);
      break;
    }
    case TOO_FEW_ARGUMENTS: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case TOO_MANY_ARGUMENTS: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case PASS_ARRAY_TO_SCALAR: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case PASS_SCALAR_TO_ARRAY: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    case IS_TYPE_NOT_VARIABLE: {
      printf("unexpected type name '%s': expected expression\n", name2);
      break;
    }
    case ARRAY_SIZE_NEGATIVE: {
      printf("'%s' declared as an array with a negative size\n", name2);
      break;
    }
    case INCOMPATIBLE_ARRAY_DIMENSION: {
      int dimCount = (int)(*name2);
      printf("array type '");
      TypeDescriptor* des = node1->semantic_value.identifierSemanticValue.symbolTableEntry->attribute->attr.typeDescriptor;
      printType(des->properties.arrayProperties.elementType);
      for (int i = dimCount; i < des->properties.arrayProperties.dimension; i++) {
        printf("[%d]", des->properties.arrayProperties.sizeInEachDimension[i]);
      }
      printf("' is not assignable");
      break;
    }
    default: {
      printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
      break;
    }
  }
}

void printErrorMsg(AST_NODE* node, ErrorMsgKind errorMsgKind) {
  g_anyErrorOccur = 1;
  // printf("Error found in line %d\n", node1->linenumber);

  printf("%s:%d: ", srcPath, node->linenumber);
  printf(ANSI_COLOR_RED "error: " ANSI_COLOR_RESET);

  switch (errorMsgKind) {
    case ARRAY_SUBSCRIPT_NOT_INT: {
      puts("array subscript is not an integer");
      break;
    }
    case NOT_ARRAY: {
      puts("subscripted value is not an array, pointer, or vector");
      break;
    }
    case ARRAY_SIZE_NOT_INT: {
      puts("size of array has non-integer type 'double'");
      break;
    }
    default: {
      printf("Unhandled case in void printErrorMsg(AST_NODE* node, ERROR_MSG_KIND* errorMsgKind)\n");
      break;
    }
  }
}

void semanticAnalysis(AST_NODE* root) {
  processProgramNode(root);
}

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2) {
  if (dataType1 == NONE_TYPE || dataType2 == NONE_TYPE) {
    return NONE_TYPE;
  }
  if (dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE) {
    return FLOAT_TYPE;
  } else {
    return INT_TYPE;
  }
}

void processProgramNode(AST_NODE* programNode) {
  initializeSymbolTable();
  AST_NODE* node = programNode->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      variableDeclareList(node);
    } else if (node->nodeType == DECLARATION_NODE) {
    }
    node = node->rightSibling;
  }
  printAllTable();
}

void processDeclarationNode(AST_NODE* declarationNode) {
  switch (declarationNode->semantic_value.declSemanticValue.kind) {
    case VARIABLE_DECL: {
      declareIdList(declarationNode->child, VARIABLE_ATTRIBUTE, 0);
      break;
    }
    case TYPE_DECL: {
      declareIdList(declarationNode->child, TYPE_ATTRIBUTE, 0);
      break;
    }
    case FUNCTION_DECL: {
      declareFunction(declarationNode->child);
      break;
    }
    case FUNCTION_PARAMETER_DECL: {
      break;
    }
    default: {
      break;
    }
  }
}

void processTypeNode(AST_NODE* idNodeAsType) {
}

void declareIdList(AST_NODE* idNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize) {
  AST_NODE* node = idNode;
  DATA_TYPE data_type = NONE_TYPE;
  SymbolTableEntry* type_entry = NULL;
  int is_type_array = 0;
  if (strcmp("int", node->semantic_value.identifierSemanticValue.identifierName) == 0) {
    data_type = INT_TYPE;
  } else if (strcmp("float", node->semantic_value.identifierSemanticValue.identifierName) == 0) {
    data_type = FLOAT_TYPE;
  } else {
    type_entry = retrieveSymbol(node->semantic_value.identifierSemanticValue.identifierName);
    if (type_entry == NULL) {
      printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
    } else {
      if (type_entry->attribute->attributeKind != TYPE_ATTRIBUTE) {
        printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_IS_NOT_TYPE);
      } else {
        if (type_entry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
          data_type = type_entry->attribute->attr.typeDescriptor->properties.dataType;
        } else {
          is_type_array = 1;
          data_type = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
        }
      }
    }
  }
  node = node->rightSibling;
  while (node) {
    if (is_type_array == 0) {
      if (node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
        TypeDescriptor* type_descriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
        type_descriptor->kind = SCALAR_TYPE_DESCRIPTOR;
        type_descriptor->properties.dataType = data_type;
        SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
        symbol_attr->attributeKind = isVariableOrTypeAttribute;
        symbol_attr->attr.typeDescriptor = type_descriptor;
        if (declaredLocally(node->semantic_value.identifierSemanticValue.identifierName)) {
          printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
        } else {
          node->semantic_value.identifierSemanticValue.symbolTableEntry =
              enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
        }
      } else if (node->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
        TypeDescriptor* type_descriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
        type_descriptor->kind = ARRAY_TYPE_DESCRIPTOR;
        int dimension = 0;
        AST_NODE* dimensionNode = node->child;
        while (dimensionNode) {
          int const_int;
          float const_float;
          DATA_TYPE type = getExprOrConstValue(dimensionNode, &const_int, &const_float);
          if (type == INT_TYPE) {
            type_descriptor->properties.arrayProperties.sizeInEachDimension[dimension] = const_int;
            dimension++;
            if (const_int < 0) {
              printErrorMsgSpecial(dimensionNode, node->semantic_value.identifierSemanticValue.identifierName, ARRAY_SIZE_NEGATIVE);
            }
          } else if (type == FLOAT_TYPE) {
            printErrorMsg(dimensionNode, ARRAY_SIZE_NOT_INT);
          } else {
            // can't evaluate expression's value
          }
          dimensionNode = dimensionNode->rightSibling;
        }
        if (declaredLocally(node->semantic_value.identifierSemanticValue.identifierName)) {
          printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
        } else {
          type_descriptor->properties.arrayProperties.dimension = dimension;
          type_descriptor->properties.arrayProperties.elementType = data_type;
          SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
          symbol_attr->attributeKind = isVariableOrTypeAttribute;
          symbol_attr->attr.typeDescriptor = type_descriptor;
          node->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
        }
      }
    } else {
      TypeDescriptor* type_descriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
      type_descriptor->kind = ARRAY_TYPE_DESCRIPTOR;
      if (node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
        SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
        symbol_attr->attributeKind = isVariableOrTypeAttribute;
        symbol_attr->attr.typeDescriptor = type_descriptor;
        int dimension = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
        for (int i = 0; i < dimension; i++) {
          type_descriptor->properties.arrayProperties.sizeInEachDimension[i] = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
        }
        type_descriptor->properties.arrayProperties.dimension = dimension;
        if (declaredLocally(node->semantic_value.identifierSemanticValue.identifierName)) {
          printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
        } else {
          node->semantic_value.identifierSemanticValue.symbolTableEntry =
              enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
        }
      } else if (node->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
        AST_NODE* dimensionNode = node->child;
        int dimension = 0;
        while (dimensionNode) {
          int const_int;
          float const_float;
          DATA_TYPE type = getExprOrConstValue(dimensionNode, &const_int, &const_float);
          if (type == INT_TYPE) {
            type_descriptor->properties.arrayProperties.sizeInEachDimension[dimension] = const_int;
            dimension++;
            if (const_int < 0) {
              printErrorMsgSpecial(dimensionNode, node->semantic_value.identifierSemanticValue.identifierName, ARRAY_SIZE_NEGATIVE);
            }
          } else if (type == FLOAT_TYPE) {
            printErrorMsg(dimensionNode, ARRAY_SIZE_NOT_INT);
          } else {
            // can't evaluate constant expression's value
          }
          dimensionNode = dimensionNode->rightSibling;
        }
        for (int i = 0; i < type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension; i++) {
          type_descriptor->properties.arrayProperties.sizeInEachDimension[i + dimension] = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
        }
        dimension += type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
        if (declaredLocally(node->semantic_value.identifierSemanticValue.identifierName)) {
          printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
        } else {
          type_descriptor->properties.arrayProperties.dimension = dimension;
          type_descriptor->properties.arrayProperties.elementType = data_type;
          SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
          symbol_attr->attributeKind = isVariableOrTypeAttribute;
          symbol_attr->attr.typeDescriptor = type_descriptor;
          node->semantic_value.identifierSemanticValue.symbolTableEntry = enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
        }
      }
    }
    node = node->rightSibling;
  }
}

void variableDeclareList(AST_NODE* declarationNode) {
  AST_NODE* node = declarationNode->child;
  while (node) {
    assert(node->nodeType == DECLARATION_NODE);
    processDeclarationNode(node);
    node = node->rightSibling;
  }
}

void declareFunction(AST_NODE* declarationNode) {
}

void parameterList() {
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode) {
}

void checkWhileStmt(AST_NODE* whileNode) {
}

void checkForStmt(AST_NODE* forNode) {
}

void checkAssignmentStmt(AST_NODE* assignmentNode) {
}

void checkIfStmt(AST_NODE* ifNode) {
}

void checkWriteFunction(AST_NODE* functionCallNode) {
}

void checkFunctionCall(AST_NODE* functionCallNode) {
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter) {
}

void processExprRelatedNode(AST_NODE* exprRelatedNode) {
}

DATA_TYPE getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue) {
  if (exprOrConstNode->nodeType == CONST_VALUE_NODE) {
    processConstValueNode(exprOrConstNode);
    if (exprOrConstNode->dataType == INT_TYPE) {
      *iValue = exprOrConstNode->semantic_value.const1->const_u.intval;
      return INT_TYPE;
    } else {
      *fValue = exprOrConstNode->semantic_value.const1->const_u.fval;
      return FLOAT_TYPE;
    }
  }
  assert(exprOrConstNode->nodeType == EXPR_NODE);
  processExprNode(exprOrConstNode);
  if (!exprOrConstNode->semantic_value.exprSemanticValue.isConstEval) {
    return NONE_TYPE;
  }
  if (exprOrConstNode->dataType == INT_TYPE) {
    *iValue = exprOrConstNode->semantic_value.exprSemanticValue.constEvalValue.iValue;
    return INT_TYPE;
  } else {
    *fValue = exprOrConstNode->semantic_value.exprSemanticValue.constEvalValue.fValue;
    return FLOAT_TYPE;
  }
}

void evaluateExprValue(AST_NODE* exprNode) {
  assert(exprNode->nodeType == EXPR_NODE);
  processExprNode(exprNode);
}

void processExprNode(AST_NODE* exprNode) {
  assert(exprNode->nodeType == EXPR_NODE);
  EXPRSemanticValue* semanticValue = &(exprNode->semantic_value.exprSemanticValue);
  if (semanticValue->kind == BINARY_OPERATION) {
    AST_NODE* child1 = exprNode->child;
    AST_NODE* child2 = child1->rightSibling;

    int isConst1, isConst2, i1, i2;
    float f1, f2;
    if (child1->nodeType == CONST_VALUE_NODE) {
      processConstValueNode(child1);
      isConst1 = 1;
      if (child1->dataType == INT_TYPE)
        i1 = child1->semantic_value.const1->const_u.intval;
      else
        f1 = child1->semantic_value.const1->const_u.fval;
    } else if (child1->nodeType == EXPR_NODE) {
      processExprNode(child1);
      isConst1 = child1->semantic_value.exprSemanticValue.isConstEval;
      if (child1->dataType == INT_TYPE)
        i1 = child1->semantic_value.exprSemanticValue.constEvalValue.iValue;
      else
        f1 = child1->semantic_value.exprSemanticValue.constEvalValue.fValue;
    } else {
      assert(child1->nodeType == IDENTIFIER_NODE);
      processVariableValue(child1);
      isConst1 = 0;
    }

    if (child2->nodeType == CONST_VALUE_NODE) {
      processConstValueNode(child2);
      isConst2 = 1;
      if (child2->dataType == INT_TYPE)
        i2 = child2->semantic_value.const1->const_u.intval;
      else
        f2 = child2->semantic_value.const1->const_u.fval;
    } else if (child2->nodeType == EXPR_NODE) {
      processExprNode(child2);
      isConst2 = child2->semantic_value.exprSemanticValue.isConstEval;
      if (child2->dataType == INT_TYPE)
        i2 = child2->semantic_value.exprSemanticValue.constEvalValue.iValue;
      else
        f2 = child2->semantic_value.exprSemanticValue.constEvalValue.fValue;
    } else {
      assert(child2->nodeType == IDENTIFIER_NODE);
      processVariableValue(child2);
      isConst2 = 0;
      return;
    }

    semanticValue->isConstEval = isConst1 && isConst2;

    // constant folding
    if (semanticValue->isConstEval) {
      switch (semanticValue->op.binaryOp) {
        case BINARY_OP_ADD: {
          exprNode->dataType = getBiggerType(child1->dataType, child2->dataType);
          if (exprNode->dataType == FLOAT_TYPE)
            semanticValue->constEvalValue.fValue =
                (child1->dataType == INT_TYPE ? i1 : f1) + (child2->dataType == INT_TYPE ? i2 : f2);
          else
            semanticValue->constEvalValue.iValue =
                (child1->dataType == INT_TYPE ? i1 : f1) + (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_SUB: {
          exprNode->dataType = getBiggerType(child1->dataType, child2->dataType);
          if (exprNode->dataType == FLOAT_TYPE)
            semanticValue->constEvalValue.fValue =
                (child1->dataType == INT_TYPE ? i1 : f1) - (child2->dataType == INT_TYPE ? i2 : f2);
          else
            semanticValue->constEvalValue.iValue =
                (child1->dataType == INT_TYPE ? i1 : f1) - (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_MUL: {
          exprNode->dataType = getBiggerType(child1->dataType, child2->dataType);
          if (exprNode->dataType == FLOAT_TYPE)
            semanticValue->constEvalValue.fValue =
                (child1->dataType == INT_TYPE ? i1 : f1) * (child2->dataType == INT_TYPE ? i2 : f2);
          else
            semanticValue->constEvalValue.iValue =
                (child1->dataType == INT_TYPE ? i1 : f1) * (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_DIV: {
          exprNode->dataType = getBiggerType(child1->dataType, child2->dataType);
          if (exprNode->dataType == FLOAT_TYPE)
            semanticValue->constEvalValue.fValue =
                (child1->dataType == INT_TYPE ? i1 : f1) / (child2->dataType == INT_TYPE ? i2 : f2);
          else
            semanticValue->constEvalValue.iValue =
                (child1->dataType == INT_TYPE ? i1 : f1) / (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_EQ: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) == (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_GE: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) >= (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_LE: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) <= (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_NE: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) != (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_GT: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) > (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_LT: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) < (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_AND: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) && (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
        case BINARY_OP_OR: {
          exprNode->dataType = INT_TYPE;
          semanticValue->constEvalValue.iValue = (child1->dataType == INT_TYPE ? i1 : f1) || (child2->dataType == INT_TYPE ? i2 : f2);
          break;
        }
      }
    }
  } else {  // unary operation
    AST_NODE* child1 = exprNode->child;
    int i1;
    float f1;
    if (child1->nodeType == CONST_VALUE_NODE) {
      processConstValueNode(child1);
      semanticValue->isConstEval = 1;
      if (child1->dataType == INT_TYPE)
        i1 = child1->semantic_value.const1->const_u.intval;
      else
        f1 = child1->semantic_value.const1->const_u.fval;
    } else if (child1->nodeType == EXPR_NODE) {
      processExprNode(child1);
      semanticValue->isConstEval = child1->semantic_value.exprSemanticValue.isConstEval;
      if (child1->dataType == INT_TYPE)
        i1 = child1->semantic_value.exprSemanticValue.constEvalValue.iValue;
      else
        f1 = child1->semantic_value.exprSemanticValue.constEvalValue.fValue;
    } else {
      assert(child1->nodeType == IDENTIFIER_NODE);
      processVariableValue(child1);
      semanticValue->isConstEval = 0;
      return;
    }
    exprNode->dataType = child1->dataType;

    // constant folding
    if (semanticValue->isConstEval) {
      switch (semanticValue->op.unaryOp) {
        case UNARY_OP_POSITIVE: {
          if (exprNode->dataType == INT_TYPE)
            semanticValue->constEvalValue.iValue = i1;
          else
            semanticValue->constEvalValue.fValue = f1;
          break;
        }
        case UNARY_OP_NEGATIVE: {
          if (exprNode->dataType == INT_TYPE)
            semanticValue->constEvalValue.iValue = -i1;
          else
            semanticValue->constEvalValue.fValue = -f1;
          break;
        }
        case UNARY_OP_LOGICAL_NEGATION: {
          if (exprNode->dataType == INT_TYPE)
            semanticValue->constEvalValue.iValue = !i1;
          else
            semanticValue->constEvalValue.fValue = !f1;
          break;
        }
      }
    }
  }
}

void processVariableValue(AST_NODE* idNode) {
  IdentifierSemanticValue* semanticValue = &(idNode->semantic_value.identifierSemanticValue);
  semanticValue->symbolTableEntry = retrieveSymbol(semanticValue->identifierName);
  if (!(idNode->semantic_value.identifierSemanticValue.symbolTableEntry)) {
    printErrorMsgSpecial(idNode, semanticValue->identifierName, SYMBOL_UNDECLARED);
    return;
  }
  SymbolAttribute* attribute = idNode->semantic_value.identifierSemanticValue.symbolTableEntry->attribute;
  if (attribute->attributeKind == TYPE_ATTRIBUTE) {  // is type
    printErrorMsgSpecial(idNode, semanticValue->identifierName, IS_TYPE_NOT_VARIABLE);
    return;
  }
  if (attribute->attributeKind == FUNCTION_SIGNATURE) {  // is function
    checkFunctionCall(idNode);
    return;
  }
  // is variable
  if (attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
    if (semanticValue->kind == ARRAY_ID) {
      printErrorMsg(idNode, NOT_ARRAY);
    }
    idNode->dataType = attribute->attr.typeDescriptor->properties.dataType;
  } else {  // array type
    int dimCount = 0;
    AST_NODE* ptr = idNode->child;
    while (ptr) {
      dimCount++;
      ptr = ptr->rightSibling;
    }
    if (dimCount > attribute->attr.typeDescriptor->properties.arrayProperties.dimension) {
      char c = (char)dimCount;
      printErrorMsgSpecial(idNode, &c, INCOMPATIBLE_ARRAY_DIMENSION);
    }
    if (dimCount < attribute->attr.typeDescriptor->properties.arrayProperties.dimension) {
      printErrorMsg(idNode, NOT_ARRAY);
    }
    idNode->dataType = attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
  }
}

// void processVariableRValue(AST_NODE* idNode) {
//   if (!(idNode->semantic_value.identifierSemanticValue.symbolTableEntry)) {
//     printErrorMsgSpecial(idNode, idNode->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
//   }
// }

void processConstValueNode(AST_NODE* constValueNode) {
  constValueNode->dataType = (constValueNode->semantic_value.const1->const_type == INTEGERC) ? INT_TYPE : FLOAT_TYPE;
}

void checkReturnStmt(AST_NODE* returnNode) {
}

void processBlockNode(AST_NODE* blockNode) {
}

void processStmtNode(AST_NODE* stmtNode) {
}

void processGeneralNode(AST_NODE* node) {
}

void processDeclDimList(AST_NODE* idNode, TypeDescriptor* typeDescriptor, int ignoreFirstDimSize) {
}
