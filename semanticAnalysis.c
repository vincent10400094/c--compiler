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
int processIdNode(AST_NODE* node, TypeDescriptor** type_descriptor, int is_type_array, SymbolTableEntry* type_entry);
int processDeclDimList(AST_NODE* arrayNameNode, TypeDescriptor** typeDescriptor, int* dimension, int ignoreFirstDimSize);
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
void processVariableLValue(AST_NODE* idNode);
void processVariableRValue(AST_NODE* idNode);
void processConstValueNode(AST_NODE* constValueNode);
DATA_TYPE getExprOrConstValue(AST_NODE* exprOrConstNode, int* iValue, float* fValue);
void evaluateExprValue(AST_NODE* exprNode);
void processParameterList(AST_NODE* parameterListNode, Parameter** parameterList, int* parametersCount);

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
  PASS_SCALAR_TO_ARRAY,
  INITIALIZER_NOT_CONSTANT
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
      printf("' is not assignable\n");
      break;
    }
    case PARAMETER_TYPE_UNMATCH: {
      printf("no matching function for call '%s'\n", name2);
      break;
    }
    default: {
      printf("Unhandled error: %d\n", errorMsgKind);
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
    case INITIALIZER_NOT_CONSTANT: {
      puts("initializer element is not a compile-time constant");
      break;
    }
    default: {
      printf("Unhandled error: %d\n", errorMsgKind);
      break;
    }
  }
}

void semanticAnalysis(AST_NODE* root) {
  processProgramNode(root);
}

DATA_TYPE getBiggerType(DATA_TYPE dataType1, DATA_TYPE dataType2) {
  if (dataType1 == NONE_TYPE || dataType2 == NONE_TYPE)
    return NONE_TYPE;
  if (dataType1 == FLOAT_TYPE || dataType2 == FLOAT_TYPE)
    return FLOAT_TYPE;
  else
    return INT_TYPE;
}

void processProgramNode(AST_NODE* programNode) {
  initializeSymbolTable();
  AST_NODE* node = programNode->child;
  while (node) {
    if (node->nodeType == VARIABLE_DECL_LIST_NODE) {
      variableDeclareList(node);
    } else if (node->nodeType == DECLARATION_NODE) {
      processDeclarationNode(node);
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

DATA_TYPE getDeclareType(AST_NODE* node, SymbolTableEntry** type_entry, int* is_typedef_array) {
  DATA_TYPE data_type = NONE_TYPE;
  if (strcmp("int", node->semantic_value.identifierSemanticValue.identifierName) == 0) {
    data_type = INT_TYPE;
  } else if (strcmp("float", node->semantic_value.identifierSemanticValue.identifierName) == 0) {
    data_type = FLOAT_TYPE;
  } else if (strcmp("void", node->semantic_value.identifierSemanticValue.identifierName) == 0) {
    data_type = VOID_TYPE;
  } else {
    *type_entry = retrieveSymbol(node->semantic_value.identifierSemanticValue.identifierName);
    if (type_entry == NULL) {
      printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
    } else {
      if ((*type_entry)->attribute->attributeKind != TYPE_ATTRIBUTE) {
        printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_IS_NOT_TYPE);
      } else {
        if ((*type_entry)->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
          data_type = (*type_entry)->attribute->attr.typeDescriptor->properties.dataType;
        } else {
          (*is_typedef_array) = 1;
          data_type = (*type_entry)->attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
        }
      }
    }
  }
  return data_type;
}

int processIdNode(AST_NODE* node, TypeDescriptor** type_descriptor, int is_type_array, SymbolTableEntry* type_entry) {
  if (is_type_array == 0) {
    if (node->semantic_value.identifierSemanticValue.kind == NORMAL_ID || node->semantic_value.identifierSemanticValue.kind == WITH_INIT_ID) {
      (*type_descriptor)->kind = SCALAR_TYPE_DESCRIPTOR;
    } else if (node->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
      (*type_descriptor)->kind = ARRAY_TYPE_DESCRIPTOR;
      int dimension = 0;
      int array_declaration = processDeclDimList(node, type_descriptor, &dimension, 0);
      if (array_declaration != 0) {
        (*type_descriptor)->properties.arrayProperties.dimension = dimension;
      } else {
        return 0;
      }
    }
  } else {
    (*type_descriptor)->kind = ARRAY_TYPE_DESCRIPTOR;
    if (node->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
      int dimension = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
      for (int i = 0; i < dimension; i++) {
        (*type_descriptor)->properties.arrayProperties.sizeInEachDimension[i] = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
      }
      (*type_descriptor)->properties.arrayProperties.dimension = dimension;
    } else if (node->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
      int dimension = 0;
      int array_declaration = processDeclDimList(node, type_descriptor, &dimension, 0);
      for (int i = 0; i < type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension; i++) {
        (*type_descriptor)->properties.arrayProperties.sizeInEachDimension[i + dimension] = type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.sizeInEachDimension[i];
      }
      dimension += type_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension;
      if (array_declaration != 0) {
        (*type_descriptor)->properties.arrayProperties.dimension = dimension;
      } else {
        return 0;
      }
    }
  }
  return 1;
}

void declareIdList(AST_NODE* idNode, SymbolAttributeKind isVariableOrTypeAttribute, int ignoreArrayFirstDimSize) {
  AST_NODE* node = idNode;
  DATA_TYPE data_type = NONE_TYPE;
  SymbolTableEntry* type_entry = NULL;
  int is_type_array = 0;
  data_type = getDeclareType(node, &type_entry, &is_type_array);
  node = node->rightSibling;
  while (node) {
    TypeDescriptor* type_descriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
    type_descriptor->properties.dataType = data_type;
    int cheak_id_node = processIdNode(node, &type_descriptor, is_type_array, type_entry);
    if (cheak_id_node) {
      SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
      symbol_attr->attributeKind = isVariableOrTypeAttribute;
      symbol_attr->attr.typeDescriptor = type_descriptor;
      if (declaredLocally(node->semantic_value.identifierSemanticValue.identifierName)) {
        printErrorMsgSpecial(node, node->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
      } else {
        node->semantic_value.identifierSemanticValue.symbolTableEntry =
            enterSymbol(node->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
      }
      node = node->rightSibling;
    }
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

void declareFunction(AST_NODE* idNode) {
  SymbolAttribute* symbol_attr = (SymbolAttribute*)malloc(sizeof(SymbolAttribute));
  symbol_attr->attributeKind = FUNCTION_SIGNATURE;
  symbol_attr->attr.functionSignature = (FunctionSignature*)malloc(sizeof(FunctionSignature));
  SymbolTableEntry* type_entry = NULL;
  int is_type_array = 0;
  symbol_attr->attr.functionSignature->returnType = getDeclareType(idNode, &type_entry, &is_type_array);
  AST_NODE* funtionNameNode = idNode->rightSibling;
  AST_NODE* parameterListNode = funtionNameNode->rightSibling;

  if (is_type_array) {
    printErrorMsg(idNode, RETURN_ARRAY);
  }
  processParameterList(parameterListNode, &(symbol_attr->attr.functionSignature->parameterList), &(symbol_attr->attr.functionSignature->parametersCount));
  if (declaredLocally(funtionNameNode->semantic_value.identifierSemanticValue.identifierName)) {
    printErrorMsgSpecial(funtionNameNode, funtionNameNode->semantic_value.identifierSemanticValue.identifierName, SYMBOL_REDECLARE);
  } else {
    funtionNameNode->semantic_value.identifierSemanticValue.symbolTableEntry =
        enterSymbol(funtionNameNode->semantic_value.identifierSemanticValue.identifierName, symbol_attr);
  }
  processBlockNode(parameterListNode->rightSibling);
}

void processParameterList(AST_NODE* parameterListNode, Parameter** parameterList, int* parametersCount) {
  AST_NODE* node = parameterListNode->child;
  if (!node) {
    (*parameterList) = NULL;
    (*parametersCount) = 0;
  } else {
    Parameter* prev = NULL;
    while (node) {
      AST_NODE* idNode = node->child;
      (*parametersCount)++;
      Parameter* parameter = (Parameter*)malloc(sizeof(Parameter));
      if (prev != NULL) {
        prev->next = parameter;
      } else {
        (*parameterList) = parameter;
      }
      prev = parameter;
      parameter->next = NULL;
      parameter->parameterName = idNode->rightSibling->semantic_value.identifierSemanticValue.identifierName;
      SymbolTableEntry* type_entry;
      int is_type_array = 0;
      getDeclareType(idNode, &type_entry, &is_type_array);
      TypeDescriptor* type_descriptor = (TypeDescriptor*)malloc(sizeof(TypeDescriptor));
      processIdNode(idNode->rightSibling, &type_descriptor, is_type_array, type_entry);
      parameter->type = type_descriptor;
      node = node->rightSibling;
    }
  }
}

void checkAssignOrExpr(AST_NODE* assignOrExprRelatedNode) {
}

void checkWhileStmt(AST_NODE* whileNode) {
  AST_NODE* test = whileNode->child;
  processExprNode(test);
  processStmtNode(test->rightSibling);
}

void checkForStmt(AST_NODE* forNode) {
  AST_NODE* listNode = forNode->child;
  AST_NODE* child;
  // initialization
  child = listNode->child;
  while (child) {
    processStmtNode(child);
    child = child->rightSibling;
  }
  // loop condition
  listNode = listNode->rightSibling;
  child = listNode->child;
  while (child) {
    processExprNode(child);
    child = child->rightSibling;
  }
  // increment or decrement
  listNode = listNode->rightSibling;
  child = listNode->child;
  while (child) {
    processStmtNode(child);
    child = child->rightSibling;
  }
  // for statement
  processStmtNode(listNode->rightSibling);
}

void checkAssignmentStmt(AST_NODE* assignmentNode) {
  AST_NODE* LHS = assignmentNode->child;
  processVariableLValue(LHS);
  AST_NODE* RHS = LHS->rightSibling;
  processExprNode(RHS);
}

void checkIfStmt(AST_NODE* ifNode) {
  AST_NODE* test = ifNode->child;
  processExprNode(test);
  AST_NODE* ifStmt = test->rightSibling;
  processStmtNode(ifStmt);
  // there is "else" statement
  if (ifStmt->rightSibling) {
    processStmtNode(ifStmt->rightSibling);
  }
}

void checkWriteFunction(AST_NODE* functionCallNode) {
}

void checkFunctionCall(AST_NODE* functionCallNode) {
  AST_NODE* functionIdNode = functionCallNode->child;
  AST_NODE* parameterListNode = functionIdNode->rightSibling;
  SymbolTableEntry* table_entry = retrieveSymbol(functionIdNode->semantic_value.identifierSemanticValue.identifierName);
  if (table_entry == NULL) {
    printErrorMsgSpecial(functionIdNode, functionIdNode->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
  } else if (table_entry->attribute->attributeKind != FUNCTION_SIGNATURE) {
    printErrorMsgSpecial(functionIdNode, functionIdNode->semantic_value.identifierSemanticValue.identifierName, NOT_FUNCTION_NAME);
  } else {
    AST_NODE* parameterNode = parameterListNode->child;
    Parameter* parameter = table_entry->attribute->attr.functionSignature->parameterList;
    while (parameter) {
      if (parameterNode == NULL || parameterNode->nodeType == NUL_NODE) {
        printErrorMsgSpecial(parameterListNode, functionIdNode->semantic_value.identifierSemanticValue.identifierName, TOO_FEW_ARGUMENTS);
        break;
      }
      checkParameterPassing(parameter, parameterNode);
      parameterNode = parameterNode->rightSibling;
      parameter = parameter->next;
    }
    if (parameterNode && parameterNode->nodeType != NUL_NODE) {
      printErrorMsgSpecial(parameterListNode, functionIdNode->semantic_value.identifierSemanticValue.identifierName, TOO_MANY_ARGUMENTS);
    }
  }
}

void checkParameterPassing(Parameter* formalParameter, AST_NODE* actualParameter) {
  if (formalParameter->type->kind == SCALAR_TYPE_DESCRIPTOR) {
    if (actualParameter->nodeType == EXPR_NODE || actualParameter->nodeType == CONST_VALUE_NODE) {
      int int_value;
      float float_value;
      DATA_TYPE type = getExprOrConstValue(actualParameter, &int_value, &float_value);
    } else if (actualParameter->nodeType == IDENTIFIER_NODE) {
      SymbolTableEntry* table_entry = retrieveSymbol(actualParameter->semantic_value.identifierSemanticValue.identifierName);
      if (actualParameter->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
        if (table_entry == NULL) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
        } else if (table_entry->attribute->attributeKind == FUNCTION_SIGNATURE) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, IS_FUNCTION_NOT_VARIABLE);
        } else if (table_entry->attribute->attributeKind == TYPE_ATTRIBUTE) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, IS_TYPE_NOT_VARIABLE);
        } else if (table_entry->attribute->attr.typeDescriptor->kind == ARRAY_TYPE_DESCRIPTOR) {
          printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PASS_ARRAY_TO_SCALAR);
        }
      } else if (actualParameter->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
        if (table_entry == NULL) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
        } else if (table_entry->attribute->attributeKind != VARIABLE_ATTRIBUTE ||
                   table_entry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
          printErrorMsg(actualParameter, NOT_ARRAY);
        } else {
          TypeDescriptor* typeDescriptor;
          int dimCount = 0;
          AST_NODE* ptr = actualParameter->child;
          while (ptr) {
            processExprNode(ptr);
            dimCount++;
            ptr = ptr->rightSibling;
          }
          if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension > dimCount) {
            printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PASS_ARRAY_TO_SCALAR);
          } else if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension < dimCount) {
            printErrorMsg(actualParameter, NOT_ARRAY);
          }
        }
      }
    } else {
      // function call node
    }
  } else {
    if (actualParameter->nodeType != IDENTIFIER_NODE) {
      printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PASS_SCALAR_TO_ARRAY);
    } else {
      SymbolTableEntry* table_entry = retrieveSymbol(actualParameter->semantic_value.identifierSemanticValue.identifierName);
      if (actualParameter->semantic_value.identifierSemanticValue.kind == NORMAL_ID) {
        if (table_entry == NULL) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
        } else if (table_entry->attribute->attributeKind == FUNCTION_SIGNATURE) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, IS_FUNCTION_NOT_VARIABLE);
        } else if (table_entry->attribute->attributeKind == TYPE_ATTRIBUTE) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, IS_TYPE_NOT_VARIABLE);
        } else if (table_entry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
          printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PASS_SCALAR_TO_ARRAY);
        } else if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension != formalParameter->type->properties.arrayProperties.dimension) {
          printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PARAMETER_TYPE_UNMATCH);
        }
      } else if (actualParameter->semantic_value.identifierSemanticValue.kind == ARRAY_ID) {
        if (table_entry == NULL) {
          printErrorMsgSpecial(actualParameter, actualParameter->semantic_value.identifierSemanticValue.identifierName, SYMBOL_UNDECLARED);
        } else if (table_entry->attribute->attributeKind != VARIABLE_ATTRIBUTE ||
                   table_entry->attribute->attr.typeDescriptor->kind == SCALAR_TYPE_DESCRIPTOR) {
          printErrorMsg(actualParameter, NOT_ARRAY);
        } else {
          int dimCount = 0;
          AST_NODE* ptr = actualParameter->child;
          while (ptr) {
            processExprNode(ptr);
            dimCount++;
            ptr = ptr->rightSibling;
          }
          if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension >= dimCount) {
            if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension - dimCount != formalParameter->type->properties.arrayProperties.dimension) {
              printErrorMsgSpecial(actualParameter, actualParameter->parent->leftmostSibling->semantic_value.identifierSemanticValue.identifierName, PARAMETER_TYPE_UNMATCH);
            }
          } else if (table_entry->attribute->attr.typeDescriptor->properties.arrayProperties.dimension < dimCount) {
            printErrorMsg(actualParameter, NOT_ARRAY);
          }
        }
      }
    }
  }
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

// void evaluateExprValue(AST_NODE* exprNode) {
//   processExprNode(exprNode);
// }

void processExprNode(AST_NODE* exprNode) {
  if (exprNode->nodeType == CONST_VALUE_NODE || exprNode->nodeType == IDENTIFIER_NODE)
    return;
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
      processVariableRValue(child1);
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
      processVariableRValue(child2);
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
      processVariableRValue(child1);
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

void processVariableLValue(AST_NODE* idNode) {
  IdentifierSemanticValue* semanticValue = &(idNode->semantic_value.identifierSemanticValue);
  semanticValue->symbolTableEntry = retrieveSymbol(semanticValue->identifierName);
  if (!(semanticValue->symbolTableEntry)) {
    printErrorMsgSpecial(idNode, semanticValue->identifierName, SYMBOL_UNDECLARED);
    return;
  }
  SymbolAttribute* attribute = semanticValue->symbolTableEntry->attribute;
  if (attribute->attributeKind == TYPE_ATTRIBUTE) {  // is type
    printErrorMsgSpecial(idNode, semanticValue->identifierName, IS_TYPE_NOT_VARIABLE);
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
      processExprNode(ptr);
      dimCount++;
      ptr = ptr->rightSibling;
    }
    if (dimCount > attribute->attr.typeDescriptor->properties.arrayProperties.dimension) {
      char c = (char)dimCount;
      printErrorMsg(idNode, NOT_ARRAY);
    }
    if (dimCount < attribute->attr.typeDescriptor->properties.arrayProperties.dimension) {
      char c = (char)dimCount;
      printErrorMsgSpecial(idNode, &c, INCOMPATIBLE_ARRAY_DIMENSION);
    }
    idNode->dataType = attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
  }
}

void processVariableRValue(AST_NODE* idNode) {
  IdentifierSemanticValue* semanticValue = &(idNode->semantic_value.identifierSemanticValue);
  semanticValue->symbolTableEntry = retrieveSymbol(semanticValue->identifierName);
  if (!(semanticValue->symbolTableEntry)) {
    printErrorMsgSpecial(idNode, semanticValue->identifierName, SYMBOL_UNDECLARED);
    return;
  }
  SymbolAttribute* attribute = semanticValue->symbolTableEntry->attribute;
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
      // not handled yet
    }
    idNode->dataType = attribute->attr.typeDescriptor->properties.arrayProperties.elementType;
  }
}

void processConstValueNode(AST_NODE* constValueNode) {
  constValueNode->dataType = (constValueNode->semantic_value.const1->const_type == INTEGERC) ? INT_TYPE : FLOAT_TYPE;
}

void checkReturnStmt(AST_NODE* returnNode) {
}

void processBlockNode(AST_NODE* blockNode) {
  // empty block
  if (!blockNode->child)
    return;
  openScope();
  AST_NODE* child = blockNode->child;
  if (child->nodeType == VARIABLE_DECL_LIST_NODE) {
    variableDeclareList(child);
    if (child->rightSibling) {
      child = child->rightSibling;
      assert(child->nodeType == STMT_LIST_NODE);
      processStmtNode(child->child);
    }
  } else {
    assert(child->nodeType == STMT_LIST_NODE);
    processStmtNode(child->child);
  }
  closeScope();
}

void processStmtNode(AST_NODE* stmtNode) {
  while (stmtNode) {
    if (stmtNode->nodeType == STMT_NODE) {
      switch (stmtNode->semantic_value.stmtSemanticValue.kind) {
        case ASSIGN_STMT: {
          checkAssignmentStmt(stmtNode);
          break;
        }
        case IF_STMT: {
          checkIfStmt(stmtNode);
          break;
        }
        case FOR_STMT: {
          checkForStmt(stmtNode);
          break;
        }
        case FUNCTION_CALL_STMT: {
          checkFunctionCall(stmtNode);
          break;
        }
        case WHILE_STMT: {
          checkWhileStmt(stmtNode);
          break;
        }
        case RETURN_STMT: {
          checkReturnStmt(stmtNode);
          break;
        }
      }
    } else if (stmtNode->nodeType == BLOCK_NODE) {
      processBlockNode(stmtNode);
    }
    stmtNode = stmtNode->rightSibling;
  }
}

void processGeneralNode(AST_NODE* node) {
}

int processDeclDimList(AST_NODE* arrayNameNode, TypeDescriptor** typeDescriptor, int* dimension, int ignoreFirstDimSize) {
  AST_NODE* dimensionNode = arrayNameNode->child;
  while (dimensionNode) {
    int const_int;
    float const_float;
    if (dimensionNode->nodeType == NUL_NODE) {
      (*typeDescriptor)->properties.arrayProperties.sizeInEachDimension[(*dimension)] = -1;
      (*dimension)++;
    } else {
      DATA_TYPE type = getExprOrConstValue(dimensionNode, &const_int, &const_float);
      if (type == INT_TYPE) {
        (*typeDescriptor)->properties.arrayProperties.sizeInEachDimension[(*dimension)] = const_int;
        (*dimension)++;
        if (const_int < 0) {
          printErrorMsgSpecial(dimensionNode, arrayNameNode->semantic_value.identifierSemanticValue.identifierName, ARRAY_SIZE_NEGATIVE);
          return 0;
        }
      } else if (type == FLOAT_TYPE) {
        printErrorMsgSpecial(dimensionNode, arrayNameNode->semantic_value.identifierSemanticValue.identifierName, ARRAY_SIZE_NOT_INT);
        return 0;
      } else {
        // can't evaluate expression's value
        return 0;
      }
    }
    dimensionNode = dimensionNode->rightSibling;
  }
  return 1;
}